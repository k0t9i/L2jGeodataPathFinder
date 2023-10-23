using System.Runtime.InteropServices;

public struct Point
{
    public uint x;
    public uint y;
};

public struct PathNode
{
    public Point min;
    public Point max;
    public short z;
};

internal class Program
{
    [DllImport("L2JGeoDataPathFinder.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern uint FindPath(out IntPtr arrayPtr, string geoDataDirectory, float startX, float startY, float startZ, float endX, float endY, ushort maxPassableHeight);
    [DllImport("L2JGeoDataPathFinder.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern uint ReleasePath(IntPtr arrayPtr);

    private static void Main(string[] args)
    {
        var arrayPtr = IntPtr.Zero;
        var size = FindPath(out arrayPtr, System.IO.Directory.GetCurrentDirectory() + "\\geodata\\", 80364, 147100, -3533, 83864, 143100, 20);
        var originalArrayPtr = arrayPtr;

        var pathNodes = new List<PathNode>();
        if (size > 0)
        {
            var tableEntrySize = Marshal.SizeOf(typeof(PathNode));
            for (var i = 0; i < size; i++)
            {
                var pathNode = Marshal.PtrToStructure(arrayPtr, typeof(PathNode));
                if (pathNode != null)
                {
                    pathNodes.Add((PathNode)pathNode);
                }
                arrayPtr = new IntPtr(arrayPtr.ToInt32() + tableEntrySize);
            }
            ReleasePath(originalArrayPtr);
        }
    }
}