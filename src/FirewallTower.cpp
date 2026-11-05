#include "FirewallTower.hpp"

#include "Enemy.hpp"
#include "MakeUnique.hpp"
#include "Projectile.hpp"

namespace {

const float FIREWALL_RANGE = 2.5f;
const float FIREWALL_DAMAGE = 6.0f;
const float FIREWALL_FIRE_INTERVAL = 0.5f;
const int FIREWALL_COST = 50;
const float FIREWALL_PROJECTILE_SPEED = 8.0f;
const float UPGRADE_INTERVAL_REDUCTION = 0.1f;

} // namespace

FirewallTower::FirewallTower(const Cell& cell)
    : Tower(cell, FIREWALL_RANGE, FIREWALL_DAMAGE, FIREWALL_FIRE_INTERVAL, FIREWALL_COST) {
}

char FirewallTower::getSymbol() const {
    return 'F';
}

void FirewallTower::fire(Enemy& target, const std::vector<Enemy*>& /*enemies*/,
                         std::vector<std::unique_ptr<Projectile> >& projectiles) {
    projectiles.push_back(util::makeUnique<Projectile>(
        getPosition(), target, FIREWALL_PROJECTILE_SPEED, getDamage()));
}

void FirewallTower::applyUpgrade() {
    setFireInterval(getFireInterval() - UPGRADE_INTERVAL_REDUCTION);
}
