#include <E57Format.h>
#include <fmt/format.h>
#include <cxxopts.hpp>
#include <indicators/cursor_control.hpp>
#include <indicators/indeterminate_progress_bar.hpp>
#include <indicators/progress_bar.hpp>
#include <indicators/termcolor.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

namespace fs = std::filesystem;

namespace point_cloud_converter {

using namespace indicators;

static constexpr size_t BUFFER_SIZE = 1024;

struct Pts_point {
    double cartesian_x = 0.0;
    double cartesian_y = 0.0;
    double cartesian_z = 0.0;
    double intensity = 0.0;
    double _unused_pad_0 = 0.0;
    double _unused_pad_1 = 0.0;
    double _unused_pad_2 = 0.0;
    uint8_t color_r = 0;
    uint8_t color_g = 0;
    uint8_t color_b = 0;
    uint8_t _unused_pad_3 = 0;

    Pts_point() = default;
    Pts_point(double const x,
              double const y,
              double const z,
              double const i,
              uint8_t const r,
              uint8_t const g,
              uint8_t const b)
      : cartesian_x(x)
      , cartesian_y(y)
      , cartesian_z(z)
      , intensity(i)
      , color_r(r)
      , color_g(g)
      , color_b(b) {
    }
};

void e57_to_pts(std::string const& input_filename,
                std::string const& output_filename,
                bool const do_possible_filter) {
    fmt::print("Where we started: {} {}\n", input_filename, output_filename);
    e57::ImageFile imf{input_filename, "r"};
    e57::StructureNode root = imf.root();

    fmt::print(
      "Did I get here at all? {} {}\n", input_filename, output_filename);

    if (!root.isDefined("data3D")) {
        throw std::runtime_error{
          fmt::format("File: {}  has no 3d points", input_filename)};
        return;
    }

    std::vector<Pts_point> pts_points;
    pts_points.resize(BUFFER_SIZE);

    std::vector<uint8_t> cartesian_invalid_state;
    cartesian_invalid_state.resize(BUFFER_SIZE);

    std::vector<std::string> lines;
    lines.reserve(1024 * 1024);

    e57::VectorNode data_3d{root.get("data3D")};
    for (int64_t scan_i = 0; scan_i < data_3d.childCount(); ++scan_i) {
        std::vector<e57::SourceDestBuffer> source_dest_buffers;

        e57::StructureNode const scan{data_3d.get(scan_i)};
        if (!scan.isDefined("points")) {
            throw std::runtime_error(
              fmt::format("Scan {} is missing points node\n", scan_i));
        }
        e57::CompressedVectorNode points{scan.get("points")};
        e57::StructureNode const prototype{points.prototype()};

        std::fill(pts_points.begin(),
                  pts_points.end(),
                  Pts_point{0.0, 0.0, 0.0, 1.0, 0, 0, 0});
        std::fill(
          cartesian_invalid_state.begin(), cartesian_invalid_state.end(), 0);

        auto add_double = [&imf, &pts_points, &prototype, &source_dest_buffers](
                            std::string const& name,
                            size_t const byte_offset,
                            bool const missing_error) {
            if (!prototype.isDefined(name)) {
                if (missing_error) {
                    throw std::runtime_error{
                      fmt::format("Missing necessary attribute: {}\n", name)};
                } else {
                    fmt::print(
                      "Warning: attribute {} is missing, will use default\n",
                      name);
                }
                return;
            }
            e57::Node const node{prototype.get(name)};
            if (node.type() != e57::E57_SCALED_INTEGER &&
                node.type() != e57::E57_FLOAT) {
                throw std::runtime_error{
                  fmt::format("Attribute: {} is the wrong type\n", name)};
            }

            uint8_t* const bytes =
              reinterpret_cast<uint8_t*>(pts_points.data());
            double* const doubles =
              reinterpret_cast<double*>(bytes + byte_offset);

            source_dest_buffers.emplace_back(
              imf, name, doubles, BUFFER_SIZE, true, true, sizeof(Pts_point));
        };

        auto add_uint8 = [&imf, &pts_points, &prototype, &source_dest_buffers](
                           std::string const& name,
                           size_t const byte_offset,
                           bool const missing_error) {
            if (!prototype.isDefined(name)) {
                if (missing_error) {
                    throw std::runtime_error{fmt::format(
                      "File is missing necessary attribute: {}\n", name)};
                } else {
                    fmt::print(
                      "Warning: attribute {} is missing, will use default\n",
                      name);
                }
                return;
            }
            e57::Node const node{prototype.get(name)};
            if (node.type() != e57::E57_INTEGER) {
                throw std::runtime_error{
                  fmt::format("Attribute: {} is the wrong type\n", name)};
            }

            uint8_t* const bytes =
              reinterpret_cast<uint8_t*>(pts_points.data()) + byte_offset;

            source_dest_buffers.emplace_back(
              imf, name, bytes, BUFFER_SIZE, true, true, sizeof(Pts_point));
        };

        add_double("cartesianX", offsetof(Pts_point, cartesian_x), true);
        add_double("cartesianY", offsetof(Pts_point, cartesian_y), true);
        add_double("cartesianZ", offsetof(Pts_point, cartesian_z), true);
        add_double("intensity", offsetof(Pts_point, intensity), false);
        add_uint8("colorRed", offsetof(Pts_point, color_r), false);
        add_uint8("colorGreen", offsetof(Pts_point, color_g), false);
        add_uint8("colorBlue", offsetof(Pts_point, color_b), false);

        if (do_possible_filter &&
            prototype.isDefined("cartesianInvalidState") &&
            prototype.get("cartesianInvalidState").type() == e57::E57_INTEGER) {
            source_dest_buffers.emplace_back(imf,
                                             "cartesianInvalidState",
                                             cartesian_invalid_state.data(),
                                             BUFFER_SIZE,
                                             true,
                                             true);
        }

        e57::CompressedVectorReader reader{points.reader(source_dest_buffers)};
        size_t total_read = 0;

        IndeterminateProgressBar point_read_bar{
          option::BarWidth{40},
          option::PostfixText{fmt::format("Reading scan: {}\r", scan_i)}};

        indicators::show_console_cursor(false);
        while (auto count = reader.read()) {
            total_read += count;
            for (int point_i = 0; point_i < count; ++point_i) {
                if (cartesian_invalid_state[point_i] == 0) {
                    auto const& pt = pts_points[point_i];
                    lines.push_back(fmt::format(
                      "{:.15f} {:.15f} {:.15f} {} {} {} {}",
                      pt.cartesian_x,
                      pt.cartesian_y,
                      pt.cartesian_z,
                      static_cast<uint8_t>(std::clamp(pt.intensity, 0.0, 1.0) *
                                           255.0),
                      pt.color_r,
                      pt.color_g,
                      pt.color_b));
                }
            }
            point_read_bar.tick();
        }
        point_read_bar.mark_as_completed();
        indicators::show_console_cursor(true);
    }

    {
        std::ofstream out_file{output_filename};
        if (!out_file) {
            throw std::runtime_error{
              fmt::format("Could not open file {} for write", output_filename)};
        }

        indicators::show_console_cursor(false);
        ProgressBar bar{option::BarWidth{40},
                        option::PostfixText{"Saving PTS file\r"}};
        auto const line_count = lines.size();
        out_file << line_count << std::endl;
        size_t total_progress_percent = 0;
        for (size_t line_i = 0; line_i < line_count; ++line_i) {
            out_file << lines[line_i] << std::endl;
            size_t const progress_percent = (100 * line_i) / line_count;
            if (progress_percent != total_progress_percent) {
                bar.tick();
                total_progress_percent = progress_percent;
            }
        }
        out_file.close();
        bar.mark_as_completed();
        indicators::show_console_cursor(true);
    }
}

}  // namespace point_cloud_converter

int main(int argc, char* argv[]) {
    std::string directory = "/usr/local/pcc";
    try {
        // clang-format off
        cxxopts::Options options{argv[0],
            " - Point Cloud Converter. "
            "Converts every .e57 file to the same named .pts file."};
        options
            //.allow_unrecognised_options()
            .add_options()
            ("dir", "directory in which to convert all .e57 files",
             cxxopts::value<std::string>(directory)
             ->default_value(directory))

            ("help", "Print help")
            ;
        // clang-format on

        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            fmt::print("{}\n", options.help({""}));
            return 0;
        }
    } catch (const cxxopts::OptionException& e) {
        fmt::print(stderr, "Error parsing options: {}\n", e.what());
        return -1;
    }

    try {
        fs::path directory_path{directory};
        if (!fs::is_directory(directory_path)) {
            throw std::runtime_error{
              fmt::format("{} is not a directory", directory)};
        }

        for (auto const& entry : fs::directory_iterator{directory_path}) {
            auto const input_file_path = entry.path();
            if (fs::is_regular_file(input_file_path) &&
                input_file_path.extension() == ".e57") {
                fmt::print("Input file: {}\n",
                           input_file_path.filename().string());

                auto output_file_path = input_file_path;
                output_file_path.replace_extension(".pts");

                point_cloud_converter::e57_to_pts(
                  input_file_path.string(), output_file_path.string(), true);
            }
        }
    } catch (std::exception const& exc) {
        std::cerr << "EXCEPTION: " << exc.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "UNKNOWN EXCEPTION" << std::endl;
        return -1;
    }

    return 0;
}
