/**
 * @file CellList.cpp
 * @author Takanori Saiki
 * @brief CellListのデータ構造を管理するクラス
 * @version 0.1
 * @date 2022-06-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "CellList.hpp"

/**
 * @brief CellListの初期化を呼びだす。
 *
 */
CellList::CellList()
{
    init();
}

CellList::~CellList()
{
}

/**
 * @brief CellListのサイズを初期化する。
 *
 */
void CellList::init()
{
    constexpr int32_t CELL_GRID_LEN_X = FIELD_X_LEN / GRID_SIZE_MAGNIFICATION;
    constexpr int32_t CELL_GRID_LEN_Y = FIELD_Y_LEN / GRID_SIZE_MAGNIFICATION;

    cellField.resize(CELL_GRID_LEN_Y);
    for (int32_t y = 0; y < CELL_GRID_LEN_Y; y++) {
        cellField[y].resize(CELL_GRID_LEN_X);
        for (int32_t x = 0; x < CELL_GRID_LEN_X; x++) {
            cellField[y][x] = std::vector<std::shared_ptr<UserCell>>();
        }
    }
}

/**
 * @brief 設定されたグリッドサイズに合わせてCellが入っているグリッドの座標(整数)を返却する。
 *
 * @param c
 * @return std::tuple<int32_t, int32_t>
 */
std::tuple<int32_t, int32_t> CellList::getGridCoordinateByCellPos(const std::shared_ptr<UserCell> c) const
{
    Vec3 pos = c->getPosition();

    const int32_t gridX = (pos.x + FIELD_X_LEN / 2) / GRID_SIZE_MAGNIFICATION;
    const int32_t gridY = (pos.y + FIELD_Y_LEN / 2) / GRID_SIZE_MAGNIFICATION;

    return std::forward_as_tuple(gridX, gridY);
}

/**
 * @brief 指定したCellの周囲にあるCellのIDリストを返す。
 *
 * @param c
 * @return std::vector<int>
 * @note CHECK_WIDTHはcalcRemoteForceのLAMBDAより大きくするのが理想。
 */
std::vector<int32_t> CellList::aroundCellList(const std::shared_ptr<UserCell> c) const
{
    std::vector<int32_t> aroundCells;
    constexpr int32_t CHECK_GRID_WIDTH = (SEARCH_RADIUS + GRID_SIZE_MAGNIFICATION - 1) / GRID_SIZE_MAGNIFICATION; // 切り上げの割り算

    auto [gridX, gridY] = getGridCoordinateByCellPos(c);

    for (int32_t y = gridY - CHECK_GRID_WIDTH; y <= gridY + CHECK_GRID_WIDTH; y++) {
        for (int32_t x = gridX - CHECK_GRID_WIDTH; x <= gridX + CHECK_GRID_WIDTH; x++) {
            if (!isInGrid(x, y)) { // グリッド外を参照している場合は飛ばす
                continue;
            }

            for (int i = 0; i < (int32_t)cellField[y][x].size(); i++) {
                if (checkInSearchRadius(c->getPosition(), cellField[y][x][i]->getPosition())) {
                    aroundCells.emplace_back(cellField[y][x][i]->arrayIndex);
                }
            }
        }
    }

    return aroundCells;
}

/**
 * @brief 指定されたグリッド座標(x, y)がグリッドの定義域に存在するかを返す。
 *
 * @param x
 * @param y
 * @return bool
 */
bool CellList::isInGrid(const int32_t x, const int32_t y) const
{
    constexpr int32_t GRID_X_WIDTH = FIELD_X_LEN / GRID_SIZE_MAGNIFICATION;
    constexpr int32_t GRID_Y_WIDTH = FIELD_Y_LEN / GRID_SIZE_MAGNIFICATION;
    // 範囲外の場合は空のvectorを返す
    if (x < 0 || GRID_X_WIDTH <= x || y < 0 || GRID_Y_WIDTH <= y) {
        return false;
    }

    return true;
}

/**
 * @brief UserCell cの範囲内にCell dが存在するかを返す。ただし、c == dのときもfalseにする。
 *
 * @param c
 * @param d
 * @return true
 * @return false
 */
bool CellList::checkInSearchRadius(const Vec3 v, const Vec3 u) const
{
    Vec3 diff            = v - u;
    const bool isInRange = (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z) <= SEARCH_RADIUS * SEARCH_RADIUS;

    // 距離がSEARCH_RADIUSより離れている場合はfalse
    if (!isInRange) {
        return false;
    }

    return true;
}

/**
 * @brief CellListに保存されているCellの配列をすべて削除する。
 *
 */
void CellList::resetGrid() noexcept
{
    constexpr int32_t GRID_X_WIDTH = FIELD_X_LEN / GRID_SIZE_MAGNIFICATION;
    constexpr int32_t GRID_Y_WIDTH = FIELD_Y_LEN / GRID_SIZE_MAGNIFICATION;

    // グリッドに保存されているCellのリストを初期化する。O(n^2) nは1辺の長さ
    for (int32_t y = 0; y < GRID_Y_WIDTH; y++) {
        for (int32_t x = 0; x < GRID_X_WIDTH; x++) {
            cellField[y][x].clear();
        }
    }
}

/**
 * @brief CellListのグリッドにCellのポインタを登録する。
 *
 * @param cell
 */
void CellList::addCell(std::shared_ptr<UserCell> cell)
{
    Vec3 pos = cell->getPosition();

    const int32_t scaledY = (pos.y + FIELD_Y_LEN / 2) / GRID_SIZE_MAGNIFICATION;
    const int32_t scaledX = (pos.x + FIELD_X_LEN / 2) / GRID_SIZE_MAGNIFICATION;

    cellField[scaledY][scaledX].emplace_back(cell);
}