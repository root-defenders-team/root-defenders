#include "EnemyFactory.hpp"

#include <stdexcept>

#include "Enemy.hpp"
#include "RootkitEnemy.hpp"
#include "VirusEnemy.hpp"
#include "WormEnemy.hpp"

std::unique_ptr<Enemy> EnemyFactory::create(
    EnemyType type, const std::vector<Vector2>& path) {
    switch (type) {
        case EnemyType::Virus:
            return std::unique_ptr<Enemy>(new VirusEnemy(path));
        case EnemyType::Worm:
            return std::unique_ptr<Enemy>(new WormEnemy(path));
        case EnemyType::Rootkit:
            return std::unique_ptr<Enemy>(new RootkitEnemy(path));
    }

    throw std::invalid_argument("Tipo de inimigo desconhecido.");
}

EnemyType EnemyFactory::parseType(const std::string& name) {
    if (name == "virus") {
        return EnemyType::Virus;
    }
    if (name == "worm") {
        return EnemyType::Worm;
    }
    if (name == "rootkit") {
        return EnemyType::Rootkit;
    }

    throw std::invalid_argument("Nome de inimigo desconhecido: " + name);
}
