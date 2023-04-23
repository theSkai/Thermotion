using Grasshopper;
using Grasshopper.Kernel;
using System;
using System.Drawing;

namespace ThermotionDesignTool2
{
    public class ThermotionDesignTool2Info : GH_AssemblyInfo
    {
        public override string Name => "ThermotionSimulationSetter";

        //Return a 24x24 pixel bitmap to represent this GHA library.
        public override Bitmap Icon => null;

        //Return a short string describing the purpose of this GHA library.
        public override string Description => "";

        public override Guid Id => new Guid("1FE136D4-4805-4910-93B2-00E2ED88F1F6");

        //Return a string identifying you or your company.
        public override string AuthorName => "";

        //Return a string representing your preferred contact details.
        public override string AuthorContact => "";
    }
}