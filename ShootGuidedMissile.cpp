#include "ShootGuidedMissile.h"

ShootGuidedMissile::ShootGuidedMissile(std::weak_ptr<ScriptBlock> block) :
    ScriptCommand(block, "shootguidedmissile")
{
}

ShootGuidedMissile::~ShootGuidedMissile()
{
    //dtor
}
