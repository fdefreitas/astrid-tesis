#include "../tesis/reconstruction3D.h"

Reconstruccion3D::Reconstruccion3D()
{

}

Reconstruccion3D::~Reconstruccion3D()
{

}

int Reconstruccion3D::getNormals()
{
    // Reads a .xyz point set file in points[].
    std::list<PointVectorPair> points;
    std::ifstream stream("puntos.off");
    if (!stream ||
        !CGAL::read_xyz_points(stream,
                               std::back_inserter(points),
                               CGAL::First_of_pair_property_map<PointVectorPair>()))
    {
        std::cerr << "No se pudo abrir el archivo puntos.off" << std::endl;
        return EXIT_FAILURE;
    }

    // Estimates normals direction.
    // Note: pca_estimate_normals() requires an iterator over points
    // as well as property maps to access each point's position and normal.
    const int nb_neighbors = 18; // K-nearest neighbors = 3 rings 18
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

int Reconstruccion3D::Delaunay()
{
    // Poisson options
    FT sm_angle = 20; // Min triangle angle in degrees.  10
    FT sm_radius = 1; // Max triangle size w.r.t. point set average spacing.   2
    FT sm_distance = 0.1; // Surface Approximation error w.r.t. point set average spacing.0.375

    // Reads the point set file in points[].
    // Note: read_xyz_points_and_normals() requires an iterator over points
    // + property maps to access each point's position and normal.
    // The position property map can be omitted here as we use iterators over Point_3 elements.
    PointList points;
    std::ifstream stream("puntosNormalizados.off");
    if (!stream ||
        !CGAL::read_xyz_points_and_normals(
                              stream,
                              std::back_inserter(points),
                              CGAL::make_normal_of_point_with_normal_pmap(std::back_inserter(points))))
    {
      std::cerr << "Error: cannot read file data/kitten.xyz" << std::endl;
      return EXIT_FAILURE;
    }

    // Creates implicit function from the read points.
    // Note: this method requires an iterator over points
    // + property maps to access each point's position and normal.
    // The position property map can be omitted here as we use iterators over Point_3 elements.
    Poisson_reconstruction_function function(
                              points.begin(), points.end(),
                              CGAL::make_normal_of_point_with_normal_pmap(points.begin()));

    // Computes the Poisson indicator function f()
    // at each vertex of the triangulation.
    if ( ! function.compute_implicit_function() )
      return EXIT_FAILURE;

    // Computes average spacing
    FT average_spacing = CGAL::compute_average_spacing(points.begin(), points.end(),
                                                       6 /* knn = 1 ring */);

    // Gets one point inside the implicit surface
    // and computes implicit function bounding sphere radius.
    Point inner_point = function.get_inner_point();
    Sphere bsphere = function.bounding_sphere();
    FT radius = std::sqrt(bsphere.squared_radius());

    // Defines the implicit surface: requires defining a
  	// conservative bounding sphere centered at inner point.
    FT sm_sphere_radius = 0.5 * radius;      //0.5
    FT sm_dichotomy_error = sm_distance*average_spacing/1000.0; // Dichotomy error must be << sm_distance
    Surface_3 surface(function,
                      Sphere(inner_point,sm_sphere_radius*sm_sphere_radius),
                      sm_dichotomy_error/sm_sphere_radius);

    // Defines surface mesh generation criteria
    CGAL::Surface_mesh_default_criteria_3<STr> criteria(sm_angle,  // Min triangle angle (degrees)
                                                        sm_radius*average_spacing,  // Max triangle size
                                                        sm_distance*average_spacing); // Approximation error

    // Generates surface mesh with manifold option
    STr tr; // 3D Delaunay triangulation for surface mesh generation
    C2t3 c2t3(tr); // 2D complex in 3D Delaunay triangulation
    CGAL::make_surface_mesh(c2t3,                                 // reconstructed mesh
                            surface,                              // implicit surface
                            criteria,                             // meshing criteria
                            CGAL::Manifold_with_boundary_tag());  // require manifold mesh

    if(tr.number_of_vertices() == 0)
      return EXIT_FAILURE;

    // saves reconstructed surface mesh
    std::ofstream out("kitten_poisson-20-30-0.375.off");
    Polyhedron output_mesh;
    CGAL::output_surface_facets_to_polyhedron(c2t3, output_mesh);
    out << output_mesh;

    return EXIT_SUCCESS;
}

