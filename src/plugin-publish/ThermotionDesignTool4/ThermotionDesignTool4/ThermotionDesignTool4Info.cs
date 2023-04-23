using Grasshopper;
using Grasshopper.Kernel;
using System;
using System.Drawing;

namespace ThermotionDesignTool4
{
    public class ThermotionDesignTool4Info : GH_AssemblyInfo
    {
        public override string Name => "ThermotionDesignTool4";

        //Return a 24x24 pixel bitmap to represent this GHA library.
        public override Bitmap Icon => null;

        //Return a short string describing the purpose of this GHA library.
        public override string Description => "";

        public override Guid Id => new Guid("1D311CF6-8267-4F39-8ABF-7905E9A74A03");

        //Return a string identifying you or your company.
        public override string AuthorName => "";

        //Return a string representing your preferred contact details.
        public override string AuthorContact => "";
    }
}