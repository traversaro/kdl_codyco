/**
 * Copyright  (C)  2013 CoDyCo Project
 * Author: Silvio Traversaro
 * website: http://www.codyco.eu
 */
  
#include "kdl_codyco/treeserialization.hpp"
#include "kdl_codyco/tree_rotation.hpp"
#include <kdl/joint.hpp>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>


namespace KDL {
    
    void TreeSerialization::addDFSrecursive(SegmentMap::const_iterator current_el,int & link_cnt)
    {
        if( current_el->second.segment.getJoint().getType() != Joint::None ) {
            joints[current_el->second.q_nr] = current_el->second.segment.getJoint().getName();
        }
        links[link_cnt] = current_el->first;
        link_cnt++;
                
        for( unsigned int i=0; i < current_el->second.children.size(); i++ ) {
            addDFSrecursive(current_el->second.children[i],link_cnt);
        }
        
    }
    
    TreeSerialization::TreeSerialization() 
    {
        links = std::vector<std::string>(0);
        joints = std::vector<std::string>(0);
    }

    TreeSerialization::~TreeSerialization() {}
    
    TreeSerialization::TreeSerialization(const Tree & tree) 
    {
        links = std::vector<std::string>(tree.getNrOfSegments());
        joints = std::vector<std::string>(tree.getNrOfJoints());
        
        
        SegmentMap::const_iterator root;
        SegmentMap::const_iterator child;
        
        int link_cnt = 0;
        
        root = tree.getRootSegment();
        for( unsigned int i=0; i < root->second.children.size(); i++ ) {
            addDFSrecursive(root->second.children[i],link_cnt);
        }
        
        assert(this->is_consistent(tree));
    }
        
    TreeSerialization::TreeSerialization(std::vector<std::string> & links_in, std::vector<std::string> & joints_in) 
    {

        links = links_in;
        joints = joints_in;
    }
    
    
    TreeSerialization::TreeSerialization(const TreeSerialization& x)
    {        
        links = std::vector<std::string>(0);
        joints = std::vector<std::string>(0);
        links = x.links;
        joints = x.joints;
    }
    
    int TreeSerialization::getJointId(std::string joint_name) const
    {
        std::vector<std::string>::const_iterator it;
        it = std::find(joints.begin(),joints.end(),joint_name);
        if( it != joints.end() ) {
            return it - joints.begin();
        } else {
            return -1;
        }
    }
        
    int TreeSerialization::getLinkId(std::string link_name) const
    {
        std::vector<std::string>::const_iterator it;
        it = std::find(links.begin(),links.end(),link_name);
        if( it != links.end() ) {
            return it - links.begin();
        } else {
            return -1;
        }
    }
    
    std::string TreeSerialization::getJointName(int joint_id) const
    {
        return joints[joint_id];
    }
    std::string TreeSerialization::getLinkName(int link_id) const
    {
        return links[link_id];
    }
    
    bool TreeSerialization::is_consistent(const Tree & tree) const
    {
        SegmentMap::const_iterator seg;
        
        if( tree.getNrOfJoints() != joints.size() || tree.getNrOfSegments() !=  links.size() ) {
            return false;
        }
        
        unsigned int i;
        
        const SegmentMap & seg_map = tree.getSegments();
        
        for(i = 0; i < links.size(); i++ ) {
            seg = tree.getSegment(links[i]);
            if( seg == seg_map.end() ) {
                return false;
            }
        }
        
        for(SegmentMap::const_iterator it=seg_map.begin(); it != seg_map.end(); it++) {
            if( it->second.segment.getJoint().getType() != Joint::None ) {
                if( getJointId( it->second.segment.getJoint().getName()) == -1 ) {
                    return false;
                }
            }
        }
        
        return true;
        
    }
            
    int TreeSerialization::getNrOfSegments() const
    {
        return links.size();
    }
        
    int TreeSerialization::getNrOfJoints() const
    {
        return joints.size();
    }
    
    bool TreeSerialization::serialize(const Tree & tree,
                                      std::vector< int> & children_root, //set of children of root
                                      std::vector< std::vector<int> > & children, //array of sets of children of each segment
                                      std::vector< int > & parent, //array of parent of each segment
                                      std::vector< int> & link2joint, //array mapping 
                                      std::vector< int > & forward_visit_order, //Visiting order for the tree, such that a parent is visited before any of his children
                                      std::vector<SegmentMap::const_iterator> & seg_vector //array of mapping between link index and SegmentMap iterators
                                      )
    {
        
        //assuming that *this and tree are consistent
        //the deprecated method is more efficient
        const SegmentMap& sm = tree.getSegments();
        
        children_root.resize(0);
        children.resize(tree.getNrOfSegments(),std::vector< int >(0));
        parent.resize(tree.getNrOfSegments());
        
        link2joint.resize(tree.getNrOfSegments(),FIXED_JOINT);
                
        seg_vector.resize(tree.getNrOfSegments());
        
        
        SegmentMap::const_iterator root, i;
        
        root = tree.getRootSegment();
        for( unsigned int j=0; j < root->second.children.size(); j++ ) {
            children_root.push_back(this->getLinkId(root->second.children[j]->first));
        }
        
        for( SegmentMap::const_iterator i=sm.begin(); i!=sm.end(); ++i ) {
            if( i != root ) {
                unsigned int i_index = this->getLinkId(i->first);
                seg_vector[i_index] = i;
                
                for( unsigned int j=0; j < i->second.children.size(); j++ ) {
                    children[i_index].push_back(this->getLinkId(i->second.children[j]->first));
                }
                
                if( i->second.segment.getJoint().getType() != Joint::None ) {
                    link2joint[i_index] = this->getJointId(i->second.segment.getJoint().getName());
                }
                
                if( i->second.parent == root ) {
                    parent[i_index] = -1;
                } else {
                    parent[i_index] = this->getLinkId(i->second.parent->first);
                }
                
            }
		}
        
        //Computing (only once, as given a root node and the structure 
        //the visiting order remains the same
        std::vector<unsigned int> index_stack;
        
        index_stack.reserve(tree.getNrOfSegments());
        forward_visit_order.reserve(tree.getNrOfSegments());
        
        index_stack.clear();
        forward_visit_order.clear();
        
        for( unsigned int j=0; j < children_root.size(); j++ ) {
            index_stack.push_back(children_root[j]);
        }
        
        while( !index_stack.empty() ) {
            
            unsigned int curr_index = index_stack.back();
            index_stack.pop_back();
            
            forward_visit_order.push_back(curr_index);
            
            //Doing the recursion on the children
            for( unsigned int j=0; j < children[curr_index].size(); j++ ) {
                index_stack.push_back(children[curr_index][j]);
            }
        }

        assert(forward_visit_order.size() == tree.getNrOfSegments());
        
        return true;
        
    }
    
    std::string TreeSerialization::toString()
    {
        std::stringstream ss;
        ss << "Link serialization:" << std::endl;
        for( int i = 0; i < (int)links.size(); i++ ) {
            ss <<  i << "\t: " << links[i] << std::endl;
        }
        ss << "Joint serialization:" << std::endl;
        for( int i = 0; i < (int)joints.size(); i++ ) {
            ss << i << "\t: " << joints[i] << std::endl;
        }
        return ss.str();
    }

}