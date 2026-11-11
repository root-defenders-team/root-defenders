#include "TowerFactory.hpp"

#include <stdexcept>

#include "AntivirusTower.hpp"
#include "FirewallTower.hpp"
#include "MakeUnique.hpp"
#include "ScannerTower.hpp"

std::unique_ptr<Tower> TowerFactory::create(TowerType type, const Cell& cell) {
    switch (type) {
        case TowerType::Firewall:
            return util::makeUnique<FirewallTower>(cell);
        case TowerType::Antivirus:
            return util::makeUnique<AntivirusTower>(cell);
        case TowerType::Scanner:
            return util::makeUnique<ScannerTower>(cell);
    }
    throw std::invalid_argument("Tipo de defesa desconhecido.");
}

TowerType TowerFactory::parseType(const std::string& name) {
    if (name == "firewall" || name == "Firewall") {
        return TowerType::Firewall;
    }
    if (name == "antivirus" || name == "Antivirus") {
        return TowerType::Antivirus;
    }
    if (name == "scanner" || name == "Scanner") {
        return TowerType::Scanner;
    }
    throw std::invalid_argument("Nome de defesa desconhecido: " + name);
}
