#include "pch.h"

#include "Entity.h"
#include "Components.h"

UUID Entity::getId() const 
{ 
    return getComponent<IdComponent>(); 
}

std::string Entity::getTag() const 
{ 
    return getComponent<TagComponent>(); 
}