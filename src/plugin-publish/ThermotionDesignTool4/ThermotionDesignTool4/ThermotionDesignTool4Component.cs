using Grasshopper;
using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Drawing;
using System.Collections.Generic;

namespace ThermotionDesignTool4
{
    public class ThermotionDesignTool4Component : GH_Component
    {
        /// <summary>
        /// Each implementation of GH_Component must provide a public 
        /// constructor without any arguments.
        /// Category represents the Tab in which the component will appear, 
        /// Subcategory the panel. If you use non-existing tab or panel names, 
        /// new tabs/panels will automatically be created.
        /// </summary>
        public ThermotionDesignTool4Component()
          : base("SimulatorClassifier", "SiC",
            "Classify components",
            "Thermotion", "Simulator")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddColourParameter("Painted Color", "Pcolor", "Color in painted area", GH_ParamAccess.list);
            pManager.AddColourParameter("UnPainted Color", "Ucolor", "Color in unpainted area", GH_ParamAccess.list);
            pManager.AddBooleanParameter("Painted Marker", "Marker", "Mark the painted area(true)", GH_ParamAccess.list);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddColourParameter("Color Output", "Color output", "the color distribution on mesh", GH_ParamAccess.list);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object can be used to retrieve data from input parameters and 
        /// to store data in output parameters.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
            List<Color> Pcolor = new List<Color>();
            List<Color> Ucolor = new List<Color>();
            List<bool> mk = new List<bool>();

            bool suc = DA.GetDataList(0, Pcolor) && DA.GetDataList(1, Ucolor) && DA.GetDataList(2, mk);
            if (!suc || mk.Count != Ucolor.Count || mk.Count != Pcolor.Count)
            {
                return;
            }
            List<Color> retList = new List<Color>();
            for(int i = 0; i < mk.Count; i++)
            {
                if (mk[i])
                {
                    retList.Add(Pcolor[i]);
                }
                else
                {
                    retList.Add(Ucolor[i]);
                }
            }
            DA.SetDataList(0, retList);
        }

        /// <summary>
        /// Provides an Icon for every component that will be visible in the User Interface.
        /// Icons need to be 24x24 pixels.
        /// You can add image files to your project resources and access them like this:
        /// return Resources.IconForThisComponent;
        /// </summary>
        protected override System.Drawing.Bitmap Icon => null;

        /// <summary>
        /// Each component must have a unique Guid to identify it. 
        /// It is vital this Guid doesn't change otherwise old ghx files 
        /// that use the old ID will partially fail during loading.
        /// </summary>
        public override Guid ComponentGuid => new Guid("916A7659-3516-4548-9D26-66E7FC18753A");
    }
}