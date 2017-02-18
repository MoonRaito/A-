public interface PathFinder
{
    /**
     * Finds the shortest route through the map and returns an array
     * of all of the cells.
     */
    public GridCell[] findPath(Map map);
}