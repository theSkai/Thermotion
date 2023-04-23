using Grasshopper;
using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.IO;

namespace ThermotionDesignTool
{
    public class ThermotionLinker : GH_Component
    {
        /// <summary>
        /// Each implementation of GH_Component must provide a public 
        /// constructor without any arguments.
        /// Category represents the Tab in which the component will appear, 
        /// Subcategory the panel. If you use non-existing tab or panel names, 
        /// new tabs/panels will automatically be created.
        /// </summary>
        public ThermotionLinker()
          : base("SimulatorLinker", "SL",
            "Linker to simulator",
            "Thermotion", "Simulator")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddTextParameter("Simulator Directory", "Directory", "the directory where simulation programme are stored and executed", GH_ParamAccess.item);
            pManager.AddBooleanParameter("Activator", "Activator", "Activator", GH_ParamAccess.item);
            pManager.AddIntegerParameter("Count of MeshVertices", "Count", "Count", GH_ParamAccess.item);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddNumberParameter("Outcome Temperature List", "TempList", "Temperature List", GH_ParamAccess.list);
            pManager.AddNumberParameter("Time", "Time", "Time", GH_ParamAccess.item);
            pManager.AddNumberParameter("Max Temperature", "MT", "Max Temperature", GH_ParamAccess.item);
            pManager.AddNumberParameter("Min Temperature", "mT", "Min Temperature", GH_ParamAccess.item);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object can be used to retrieve data from input parameters and 
        /// to store data in output parameters.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
            string fname = string.Empty;
            bool activated = false;
            bool success = DA.GetData(0, ref fname) && DA.GetData(1, ref activated);
            if (!success) return;
            if (!activated)
            {
                AddRuntimeMessage(GH_RuntimeMessageLevel.Blank, "Not Activated");
            }
            else
            {
                fname += @"\TempFile\Temper";
                int k = int.MinValue;
                DA.GetData(2, ref k);
                List<double> retList = new List<double>();
                double time = double.MinValue;
                double maxtemp = double.MaxValue;
                double mintemp = double.MinValue;
            tryagain:
                if (!File.Exists(fname))
                {
                    DA.SetData(1, -1);
                    return;
                }
                try
                {
                    using (FileStream sr = new FileStream(fname, FileMode.Open, FileAccess.Read, FileShare.Read))
                    {
                        using (StreamReader sr2 = new StreamReader(sr))
                        {
                            string line;
                            retList.Clear();
                            line = sr2.ReadLine();
                            time = double.Parse(line);
                            while ((line = sr2.ReadLine()) != null)
                            {
                                if (line[0] == 'n')
                                {
                                    DA.SetData(1, -2);
                                    return;
                                }
                                retList.Add(double.Parse(line));
                            }
                            if (retList.Count < 3) return;
                            maxtemp = retList[retList.Count - 2];
                            mintemp = retList[retList.Count - 1];
                            retList.RemoveAt(retList.Count - 1);
                            retList.RemoveAt(retList.Count - 1);
                            sr.Flush();
                            if (retList.Count == k)
                            {
                                DA.SetDataList(0, retList);
                                DA.SetData(1, time);
                                DA.SetData(2, maxtemp);
                                DA.SetData(3, mintemp);
                            }
                            else
                            {
                                AddRuntimeMessage(GH_RuntimeMessageLevel.Warning, "DATA error " + retList.Count.ToString());
                                return;
                            }
                        }
                    }
                }
                catch (Exception e)
                {
                    AddRuntimeMessage(GH_RuntimeMessageLevel.Blank, e.Message);
                    goto tryagain;
                }
            }
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
        public override Guid ComponentGuid => new Guid("0C858968-C9FA-4572-A93D-1AAE56496153");
    }
}