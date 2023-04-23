using Grasshopper;
using Grasshopper.Kernel;
using System;
using System.Drawing;

namespace ThermotionDesignTool3
{
    public class ThermotionDesignTool3Info : GH_AssemblyInfo
    {
        public override string Name => "ThermotionSimulatorStarter";

        //Return a 24x24 pixel bitmap to represent this GHA library.
        public override Bitmap Icon => null;

        //Return a short string describing the purpose of this GHA library.
        public override string Description => "";

        public override Guid Id => new Guid("93470FC0-D9AD-425A-9113-2DADE0C84150");

        //Return a string identifying you or your company.
        public override string AuthorName => "";

        //Return a string representing your preferred contact details.
        public override string AuthorContact => "";
    }
}