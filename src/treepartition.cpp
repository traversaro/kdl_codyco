/**
 * Copyright  (C)  2013 CoDyCo Project
 * Author: Silvio Traversaro
 * website: http://www.codyco.eu
 */
  
#include "kdl_codyco/treepartition.hpp"
#include <kdl/joint.hpp>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>


namespace KDL {
namespace CoDyCo {
    
    TreePart::TreePart(): part_id(-1), part_name("TreePartError"), dof_id(0), links_id(0)
    {
    }
    
    TreePart::TreePart(int _part_id, std::string _part_name): part_id(_part_id), part_name(_part_name), dof_id(0), links_id(0)
    {
    }
    
    TreePart::TreePart(int _part_id, std::string _part_name, std::vector<int> _dof_id, std::vector<int> _links_id): part_id(_part_id), part_name(_part_name), dof_id(_dof_id), links_id(_links_id)
    {
    }
    
    TreePart::~TreePart()
    {
    }
            
    int TreePart::getNrOfLinks() const
    {
        return links_id.size();
    }
    
    int TreePart::getNrOfDOFs() const
    {
        return dof_id.size();
    }
            
    std::string TreePart::getPartName() const
    {
        return part_name;
    }
    
    int TreePart::getPartID() const
    {
        return part_id;
    }
    
    TreePartition::TreePartition()
    {
    } 
        
    TreePartition::~TreePartition()
    {
    }
    
    TreePartition::TreePartition(const Tree & tree)
    {
        int nrOfDOFs = tree.getNrOfJoints();
        int nrOfLinks = tree.getNrOfSegments();
        
        TreePart tree_part(0,"default_part");
        
        for(int i=0; i <nrOfDOFs; i++ )
        {
            tree_part.addDOF(i);
        }
        
        for(int i=0; i <nrOfLinks; i++ )
        {
            tree_part.addLink(i);
        }
        
        addPart(tree_part);
    }

    bool TreePartition::addPart(TreePart & tree_part)
    {
        parts.push_back(tree_part);
        return true;
    }
        
    TreePart TreePartition::getPart(int id) const
    {
        int local_index;
        
        std::map<int,int>::const_iterator it = ID_map.find(id);
        
        if( it == ID_map.end() ) {
            return TreePart();
        }
        
        local_index = it->second;
        
        return parts[local_index];
    }
    
    TreePart TreePartition::getPart(std::string part_name) const
    {
        int local_index;
        
        std::map<std::string,int>::const_iterator it = name_map.find(part_name);
        
        if( it == name_map.end() ) {
            return TreePart();
        }
        
        local_index = it->second;
        
        return parts[local_index];
    }
    
    int TreePartition::getPartIDfromLink(int link_id) const
    {
        for(int i=0; i < (int)parts.size(); i++ ) {
            for( int j=0; j < parts[i].getNrOfLinks(); j++ ) {
                if( parts[i].getGlobalLinkIndex(j) == link_id ) {
                    return parts[i].getPartID();
                }
            }
        }
        return -1;
    }
    
    int TreePartition::getPartIDfromDOF(int dof_id) const
    {
        for(int i=0; i < (int)parts.size(); i++ ) {
            for( int j=0; j < parts[i].getNrOfDOFs(); j++ ) {
                if( parts[i].getGlobalDOFIndex(j) == dof_id ) {
                    return parts[i].getPartID();
                }
            }
        }
        return -1;
    }

    int TreePartition::getGlobalLinkIndex(int part_ID, int local_link_index) const
    {    
        int local_index;
        
        std::map<int,int>::const_iterator it = ID_map.find(part_ID);
        
        if( it == ID_map.end() ) {
            return -1;
        }
        
        local_index = it->second;
        
        if( local_link_index >= parts[local_index].getNrOfLinks() || local_link_index < 0 ) return -1;
        
        return parts[local_index].getGlobalLinkIndex(local_link_index);
    }
        
    int TreePartition::getGlobalDOFIndex(int part_ID, int local_DOF_index) const
    {
        int local_index;
        
        std::map<int,int>::const_iterator it = ID_map.find(part_ID);
        
        if( it == ID_map.end() ) {
            return -1;
        }
        
        local_index = it->second;
        
        return parts[local_index].getGlobalDOFIndex(local_DOF_index);
    }
        
    int TreePartition::getLocalLinkIndex(int global_link_index) const
    {
        for(int i=0; i < (int)parts.size(); i++ ) {
            for( int j=0; j < parts[i].getNrOfLinks(); j++ ) {
                if( parts[i].getGlobalLinkIndex(j) == global_link_index ) {
                    return j;
                }
            }
        }
        return -1;
    }
        
    int TreePartition::getLocalDOFIndex(int global_dof_index) const
    {
        for(int i=0; i < (int)parts.size(); i++ ) {
            for( int j=0; j < parts[i].getNrOfDOFs(); j++ ) {
                if( parts[i].getGlobalDOFIndex(j) == global_dof_index) {
                    return j;
                }
            }
        }
        return -1;
    }
    
         
    /**
    * Check if the TreePartition is a valid serialization for the 
    * given Tree (and optionally the given TreeSerialization ) 
    * 
    */
    bool TreePartition::is_consistent(const Tree & tree ) const
    {
        return is_consistent(tree,TreeSerialization(tree));
    }
    
    bool TreePartition::is_consistent(const Tree & tree, TreeSerialization tree_serialization) const
    {
        if( !tree_serialization.is_consistent(tree) ) return false;
        
        int total_links = 0;
        
        for(int i=0; i < (int)parts.size(); i++ ) {
            TreePart part = parts[i];
            if( part.getNrOfLinks() <= 0 ) return false;
            total_links +=  part.getNrOfLinks();
        }
        
        if( total_links != (int)tree.getNrOfSegments() ) return false;
        
        /**
         * \todo add link/joint id level serialization
         */
        
        return true;
    }
        
    std::string TreePartition::toString() const
    {
        return "";
    }
}
}