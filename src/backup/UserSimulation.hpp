/**
 * @file UserSimulation.hpp
 * @author Takanori Saiki
 * @brief シミュレーション固有の設計をここに記す。
 * @version 0.1
 * @date 2022-06-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include "Simulation.hpp"

class UserSimulation : public Simulation
{
    private:
    Vec3 calcCellCellForce(Cell &c) const noexcept override;

    public:
    UserSimulation(/* args */);
    ~UserSimulation();

    void initCells() noexcept override;
};