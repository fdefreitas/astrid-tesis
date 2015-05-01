#include "../tesis/normals.h"

Normals::Normals()
{

}

Normals::~Normals()
{

}


int Normals::getNormals()
{
    // Reads a .xyz point set file in points[].
    std::list<PointVectorPair> points;
    std::ifstream stream("puntos.off");
    if (!stream ||
        !CGAL::read_xyz_points(stream,
                               std::back_inserter(points),
                               CGAL::First_of_pair_property_map<PointVectorPair>()))
    {
        std::cerr << "Error: cannot read file data/sphere_20k.xyz" << std::endl;
        return EXIT_FAILURE;
    }

    // Estimates normals direction.
    // Note: pca_estimate_normals() requires an iterator over points
    // as well as property maps to access each point's position and normal.
    const int nb_neighbors = 18; // K-nearest neighbors = 3 rings
    CGAL::pca_estimate_normals(points.begin(), points.end(),
                               CGAL::First_of_pair_property_map<PointVectorPair>(),
                               CGAL::Second_of_pair_property_map<PointVectorPair>(),
                               nb_neighbors);

    // Orients normals.
    // Note: mst_orient_normals() requires an iterator over points
    // as well as property maps to access each point's position and normal.
    std::list<PointVectorPair>::iterator unoriented_points_begin =
        CGAL::mst_orient_normals(points.begin(), points.end(),
                                 CGAL::First_of_pair_property_map<PointVectorPair>(),
                                 CGAL::Second_of_pair_property_map<PointVectorPair>(),
                                 nb_neighbors);

    // Optional: delete points with an unoriented normal
    // if you plan to call a reconstruction algorithm that expects oriented normals.
    points.erase(unoriented_points_begin, points.end());

    // Optional: after erase(), use Scott Meyer's "swap trick" to trim excess capacity
    std::list<PointVectorPair>(points).swap(points);


std::list<PointVectorPair>::iterator iter = points.begin();

Point s;
Vector c;
int u = 0;

std::ofstream out("puntosNormalizados.off");

for(;iter != points.end();iter++)
{
 s = (*iter).first;
 c = (*iter).second;

out << s << " " << c << "\n";

}

out.close();

return EXIT_SUCCESS;
}