#include "ScannerTower.hpp"

#include "Enemy.hpp"
#include "Projectile.hpp"

namespace {

const float SCANNER_RANGE = 2.0f;
const float SCANNER_DAMAGE = 8.0f;
const float SCANNER_FIRE_INTERVAL = 1.0f;
const int SCANNER_COST = 120;
const float UPGRADE_RANGE_BONUS = 0.5f;

} // namespace

ScannerTower::ScannerTower(const Cell& cell)
    : Tower(cell, SCANNER_RANGE, SCANNER_DAMAGE, SCANNER_FIRE_INTERVAL, SCANNER_COST) {
}

char ScannerTower::getSymbol() const {
    return 'S';
}

void ScannerTower::fire(Enemy& /*target*/, const std::vector<Enemy*>& enemies,
                        std::vector<std::unique_ptr<Projectile> >& /*projectiles*/) {
    for (std::size_t i = 0; i < enemies.size(); ++i) {
        Enemy* enemy = enemies[i];
        if (enemy != nullptr && enemy->isActive() && !enemy->isDead() && isInRange(*enemy)) {
            enemy->takeDamage(getDamage());
        }
    }
}

void ScannerTower::applyUpgrade() {
    setRange(getRange() + UPGRADE_RANGE_BONUS);
}
