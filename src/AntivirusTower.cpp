#include "AntivirusTower.hpp"

#include "Enemy.hpp"
#include "MakeUnique.hpp"
#include "Projectile.hpp"

namespace {

const float ANTIVIRUS_RANGE = 4.0f;
const float ANTIVIRUS_DAMAGE = 30.0f;
const float ANTIVIRUS_FIRE_INTERVAL = 2.0f;
const int ANTIVIRUS_COST = 100;
const float ANTIVIRUS_PROJECTILE_SPEED = 6.0f;
const float UPGRADE_DAMAGE_BONUS = 15.0f;

} // namespace

AntivirusTower::AntivirusTower(const Cell& cell)
    : Tower(cell, ANTIVIRUS_RANGE, ANTIVIRUS_DAMAGE, ANTIVIRUS_FIRE_INTERVAL, ANTIVIRUS_COST) {
}

char AntivirusTower::getSymbol() const {
    return 'A';
}

void AntivirusTower::fire(Enemy& target, const std::vector<Enemy*>& /*enemies*/,
                          std::vector<std::unique_ptr<Projectile> >& projectiles) {
    projectiles.push_back(util::makeUnique<Projectile>(
        getPosition(), target, ANTIVIRUS_PROJECTILE_SPEED, getDamage()));
}

void AntivirusTower::applyUpgrade() {
    setDamage(getDamage() + UPGRADE_DAMAGE_BONUS);
}
