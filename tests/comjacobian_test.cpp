#include <kdl_codyco/utils.hpp>
#include <kdl_codyco/undirectedtree.hpp>

#include <kdl_codyco/rnea_loops.hpp>
#include <kdl_codyco/regressor_utils.hpp>

#include <kdl/rigidbodyinertia.hpp>
#include <kdl/frames_io.hpp>

#include <kdl_codyco/treeidsolver_recursive_newton_euler.hpp>
#include <kdl_codyco/treedynparam.hpp>
#include <kdl_codyco/floatingjntspaceinertiamatrix.hpp>
#include <kdl_codyco/treejnttocomjacsolver.hpp>

#include <kdl_codyco/position_loops.hpp>

#include "test_models.hpp"

#include <ctime>
#include <boost/concept_check.hpp>

#include <kdl_codyco/jacobian_loops.hpp>

using namespace KDL;
using namespace KDL::CoDyCo;


    void getCOMJacobianLoop(const TreeGraph & tree_graph,
                           const KDL::JntArray &q, 
                           const Traversal & traversal,
                           const std::vector<Frame>& X_b,
                           Jacobian & jac,
                           Jacobian & buffer_jac,
                           int part_id=-1)
    {
        
        assert(tree_graph.getNrOfDOFs()+6 == jac.columns());
        
        SetToZero(jac);
    
        double m = 0;
            
        for(int l=traversal.order.size()-1; l>=0; l-- ) {
            
            LinkMap::const_iterator link = traversal.order[l];

            //if all part is considered, or this link belong to the considered part
            if( part_id < 0 || part_id == (int)link->body_part_nr ) {
                //\todo improve this code, that is like o(n^2)
                //It is easy to implement a o(n) version of it
                //Get the floating base jacobian for current link (expressed in local frame)
                getFloatingBaseJacobianLoop(tree_graph,q,traversal,link->link_nr,buffer_jac);
                
                double m_i = link->I.getMass();
                
                //Change the pole of the jacobian in the link COM
                buffer_jac.changeRefPoint(link->I.getCOG());
                
                //Change the orientation to the one of the base
                buffer_jac.changeBase(X_b[link->link_nr].M);

                //Add the computed jacobian to the total one, multiplied by the link mass
                jac.data += m_i*buffer_jac.data;
                
                m += m_i;
           
            }
        }
        
        //Divide the jacobian with the normal one
        jac.data /= m;
        
        return;
    }

double random_double()
{
    return ((double)rand()-RAND_MAX/2)/((double)RAND_MAX);
}

int main()
{    
    JntArray q;
    Jacobian com_jac;
    
    srand(time(NULL));
    
    Tree test_tree = TestHumanoid();
    
    //Create a solver for com jacobian
    TreeJntToCOMJacSolver com_jac_slv(test_tree);
    
    
    //Create 
    q = JntArray(test_tree.getNrOfJoints());
    com_jac = Jacobian(test_tree.getNrOfJoints()+6);
    
    for(int i=0; i < test_tree.getNrOfJoints(); i++ )
    {
        q(i) = random_double();
    }
    
    //Inserting the random input data in both solvers, while checking all went well
    int ret = com_jac_slv.JntToJac(q,com_jac);
    if( ret != 0 ) { std::cerr << "Error " << ret << " in jacobian solver" << std::endl; return -1; }

    //Tryng to get the same result with local O(n^2) algorithm
    TreeSerialization serial = com_jac_slv.getSerialization();
    UndirectedTree und_tree = UndirectedTree(test_tree,serial);
    Traversal trav;
    und_tree.compute_traversal(trav);
    
    Jacobian jac_test(und_tree.getNrOfDOFs()+6), jac_buf(und_tree.getNrOfDOFs()+6);
    std::vector<Frame> X_b(und_tree.getNrOfLinks());
    
    getFramesLoop(und_tree,q,trav,X_b);
    getCOMJacobianLoop(und_tree,q,trav,X_b,jac_test,jac_buf);
    
    //Only the linar part of the test loop is the average velocity of the robot
    double err_norm = (com_jac.data.block(0,0,3,com_jac.data.cols())-jac_test.data.block(0,0,3,jac_test.data.cols())).norm();
    if( err_norm > 1e-10 )  return -1;
    std::cout << "Norm of diff matrix " << err_norm << std::endl;
    
    return 0;
}