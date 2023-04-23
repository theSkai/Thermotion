using Grasshopper;
using Grasshopper.Kernel;
using System;
using System.Drawing;

namespace ThermotionDesignTool
{
    public class ThermotionDesignToolInfo : GH_AssemblyInfo
    {
        public override string Name => "ThermotionSimulatorLinker";

        //Return a 24x24 pixel bitmap to represent this GHA library.
        public override Bitmap Icon => null;

        //Return a short string describing the purpose of this GHA library.
        public override string Description => "";

        public override Guid Id => new Guid("69146B64-CB14-44F6-A412-B7A16D29BDC4");

        //Return a string identifying you or your company.
        public override string AuthorName => "";

        //Return a string representing your preferred contact details.
        public override string AuthorContact => "";
    }
}