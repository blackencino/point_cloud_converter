
#include <E57Format.h>

#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace point_cloud_converter {

// look for a thing named 'blah'
if (!parent.isDefined("blah")) {
    // complain
}

auto const blah_prototype = points_prototype.get("blah");


// pts files are separated by spaces, so that's easy
vector<SourceDestBuffer> source_dest_buffers;

// double
std::vector<double> buffer;
buffer.resize(BUFFER_SIZE);
e57::SourceDestBuffer source_dest_buffer{imf, element_name, buffer.data(), BUFFER_SIZE, true, true};


std::vector<std::variant<std::vector<double>, std::vector<int64_t>>> buffers;

// if cartesianInvalidState exists, it has to be zero
outcome<e57::SourceDestBuffer> get_source_dest_buffer(e57::StructureNode const& prototype,
                                          std::string const& name, 
                                          double* const storage,
                                          size_t const capacity,
                                          )

struct Pts_point {
    double cartesian_x;
    double cartesian_y;
    double cartesian_z;
    uint8_t intensity,
    uint8_t color_r,
    uint8_t color_g,
    uint8_t color_b
};

static_assert(sizeof(Pts_point) == 4 * sizeof(double), "Pts point size");


for (int i=0; i<prototype.childCount(); ++i) {
                        Node n(prototype.get(i));
                        pointrecord += comma + n.elementName();
                        if (comma.empty()) comma = ",";
                        switch(n.type()) {
                            case e57::E57_FLOAT:
                            case e57::E57_SCALED_INTEGER:
                                source_dest_buffers.emplace_back(
                                        imf
                                        , n.elementName()
                                        , &get<vector<double> >(buf[i])[0]
                                        , buf_size
                                        , true
                                        , true
                                );
                                break;
                            case e57::E57_INTEGER:
                                source_dest_buffers.emplace_back(
                                        imf
                                        , n.elementName()
                                        , &get<vector<int64_t> >(buf[i])[0]
                                        , buf_size
                                        , true
                                        , true
                                );
                            break;
                            case e57::E57_STRING:
                                source_dest_buffers.emplace_back(
                                    SourceDestBuffer(
                                        imf
                                        , n.elementName()
                                        , &get<vector<ustring> >(buf[i])
                                    )
                                );
                            break;
                            default:
                                throw(runtime_error(
                                    "prototype contains illegal type")
                            );
                        }


void do_thing(std::string const& input_filename,
std::string const& position_x_name,
std::string const& position_y_name,
std::string const& position_z_name,
std::string const& intensity_name,
std::string const& color_r_name,
std::string const& color_g_name,
std::string const& color_b_name,

std::optional<std::tuple<std::string, double>> filter_attribute) {
    e57::ImageFile imf{input_filename, "r"};
    e57::StructureNode root = imf.root();

    if (!root.isDefined("data3D")) {
        throw std::runtime_error{"File: " + input_filename +
                                 " has no 3d points"};
        return;
    }

    e57::VectorNode data_3d{root.get("data3D")};
    for (int64_t scan_i = 0; scan_i < data_3d.childCount(); ++scan_i) {
        e57::StructureNode const scan{data_3d.get(scan_i)};
        e57::CompressedVectorNode const points{scan.get("points")};
        e57::StructureNode const prototype{points.prototype()};

        for (int64_t element_i = 0; element_i < prototype.childCount();
             ++element_i) {
            auto const element = prototype.get(element_i);
            std::cout << "element name: " << element.elementName()
                      << ", and type: " << static_cast<int>(element.type())
                      << std::endl;
        }
    }

    // vector<SourceDestBuffer> sdb;
    // const size_t buf_size = 1024;
    // vector<variant<vector<double>, vector<int64_t>, vector<ustring> > > buf;
    // string pointrecord;

    //             string comma;
    //             if (!opt.count("format")) {
    //                 fmt.clear();
    //                 for (int64_t i=0; i<prototype.childCount(); ++i) {
    //                     switch(prototype.get(i).type()) {
    //                         case e57::E57_FLOAT:
    //                         case e57::E57_SCALED_INTEGER:
    //                             buf.push_back(vector<double>(buf_size));
    //                             if (!opt.count("format"))
    //                                 fmt += comma
    //                                 +"%."+lexical_cast<string>(numeric_limits<double>::digits10)+"g";
    //                             break;
    //                         case e57::E57_INTEGER:
    //                             buf.push_back(vector<int64_t>(buf_size));
    //                             if (!opt.count("format"))
    //                                 fmt += comma +"%d";
    //                             break;
    //                         case e57::E57_STRING:
    //                             buf.push_back(vector<ustring>(buf_size));
    //                             if (!opt.count("format"))
    //                                 fmt += comma + "%s";
    //                     }
    //                     if (comma.empty()) comma = ",";
    //                 }
    //             }
    //             comma.clear();
    //             for (int i=0; i<prototype.childCount(); ++i) {
    //                 Node n(prototype.get(i));
    //                 pointrecord += comma + n.elementName();
    //                 if (comma.empty()) comma = ",";
    //                 switch(n.type()) {
    //                     case e57::E57_FLOAT:
    //                     case e57::E57_SCALED_INTEGER:
    //                         source_dest_buffers.emplace_back(
    //                             SourceDestBuffer(
    //                                 imf
    //                                 , n.elementName()
    //                                 , &get<vector<double> >(buf[i])[0]
    //                                 , buf_size
    //                                 , true
    //                                 , true
    //                             )
    //                         );
    //                         break;
    //                     case e57::E57_INTEGER:
    //                         source_dest_buffers.emplace_back(
    //                             SourceDestBuffer(
    //                                 imf
    //                                 , n.elementName()
    //                                 , &get<vector<int64_t> >(buf[i])[0]
    //                                 , buf_size
    //                                 , true
    //                                 , true
    //                             )
    //                         );
    //                     break;
    //                     case e57::E57_STRING:
    //                         source_dest_buffers.emplace_back(
    //                             SourceDestBuffer(
    //                                 imf
    //                                 , n.elementName()
    //                                 , &get<vector<ustring> >(buf[i])
    //                             )
    //                         );
    //                     break;
    //                     default:
    //                         throw(runtime_error(
    //                             "prototype contains illegal type")
    //                     );
    //                 }
    //             }

    //             CompressedVectorReader rd(points.reader(sdb));
    //             path
    //             csvname(string("image3d-")+lexical_cast<string>(child)+".csv");
    //             ofstream ocsv(dst/csvname);
    //             ostream& out(ocsv); // needed to fix ambiguity for <<
    //             operator on msvc cout << "unpacking: " << dst/csvname << "
    //             ... "; unsigned count; uint64_t total_count(0);

    //             format tfmt(fmt);
    //             tfmt.exceptions( all_error_bits ^ too_many_args_bit );
    //             out << pointrecord << endl; // put the header line into csv
    //             while(count = rd.read()) {
    //                 total_count += count;
    //                 for (size_t i=0; i<count; ++i) {
    //                     for (size_t j=0; j<buf.size(); ++j)
    //                         tfmt = tfmt % apply_visitor(get_at(i),buf.at(j));
    //                     out << tfmt << endl;
    //                 }
    //             }
    //             cout << " total points: " << total_count << endl;

    //             ocsv.close();
    //         }
}

}  // namespace point_cloud_converter

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "USAGE: " << argv[0] << " <blah.e57>" << std::endl;
        return -2;
    }
    try {
        point_cloud_converter::do_thing(argv[1]);
    } catch (std::exception const& exc) {
        std::cerr << "EXCEPTION: " << exc.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "UNKNOWN EXCEPTION" << std::endl;
        return -1;
    }

    return 0;
}
