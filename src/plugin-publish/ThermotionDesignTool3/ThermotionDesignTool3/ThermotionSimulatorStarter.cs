using Grasshopper;
using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.IO;

namespace ThermotionDesignTool3
{
    public class ThermotionSimulationStarter : GH_Component
    {
        /// <summary>
        /// Each implementation of GH_Component must provide a public 
        /// constructor without any arguments.
        /// Category represents the Tab in which the component will appear, 
        /// Subcategory the panel. If you use non-existing tab or panel names, 
        /// new tabs/panels will automatically be created.
        /// </summary>
        public ThermotionSimulationStarter()
          : base("SimulatorStarter", "SStr",
            "Setting the start circumstance of simulator programme, and clear the temporary file",
            "Thermotion", "Simulator")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddBooleanParameter("Use Decorate", "UseDecr", "Define whether use double layer", GH_ParamAccess.item); ;
            pManager.AddNumberParameter("Coefficient WaterToBase", "Coe WB", "Heat convertion per unit second per unit area per unit temperature difference: W/m^2 * K", GH_ParamAccess.item);
            pManager.AddNumberParameter("Coefficient BaseToDecorate", "Coe BD", "Heat convertion per unit second per unit area per unit temperature difference: W/m^2 * K", GH_ParamAccess.item);
            pManager.AddNumberParameter("Coefficient BaseToAir", "Coe BA", "Heat convertion per unit second per unit area per unit temperature difference: W/m^2 * K", GH_ParamAccess.item);
            pManager.AddNumberParameter("Coefficient DecorateToAir", "Coe DA", "Heat convertion per unit second per unit area per unit temperature difference: W/m^2 * K", GH_ParamAccess.item);

            pManager.AddNumberParameter("BaseLayer thickness", "Bthick", "Thickness of base layer", GH_ParamAccess.item);
            pManager.AddNumberParameter("DecorateLayer thickness", "Dthick", "Thickness of decorate layer", GH_ParamAccess.item); ;
            pManager.AddNumberParameter("Environment temperature", "EnvnTemper", "Environment temperature", GH_ParamAccess.item);
            pManager.AddNumberParameter("Water start temperature", "WtrTemper", "Water temperature", GH_ParamAccess.item);
            pManager.AddNumberParameter("Water start flow", "WtrFlow", "Water flow", GH_ParamAccess.item);
            pManager.AddNumberParameter("Base Material", "BaseMat", "Consists three parameters:\n specific heat capacity(J/kg * K)\n density(kg/m^3)\n heat conduction coefficient(W/m * K)", GH_ParamAccess.list);
            pManager.AddNumberParameter("Decorate Material", "DecrMat", "Consists three parameters:\n specific heat capacity(J/kg * K)\n density(kg/m^3)\n heat conduction coefficient(W/m * K)", GH_ParamAccess.list);
            
            pManager.AddMeshParameter("Mesh for Simulaton(BaseLayer Mesh)", "BaseMesh", "Base Area to simulate", GH_ParamAccess.item);
            pManager.AddPointParameter("DecorateLayer Mesh Points", "DecrMeshPts", "Mark the decorate area", GH_ParamAccess.list);
            pManager.AddBooleanParameter("Polyline Water?", "Plyline?", "Is the Water Polyline or Area", GH_ParamAccess.item);
            pManager.AddPointParameter("Water points", "WtrPoints", "Fit the water area", GH_ParamAccess.list);
            pManager.AddNumberParameter("Water widths", "WtrWidths", "each width of points above, when only give one value, it means the all widths are same", GH_ParamAccess.list);
            pManager.AddNumberParameter("Water section area", "WtrSecArea", "each section area of points above, when only give one value, it means all area sizes are same", GH_ParamAccess.list);

            pManager.AddBooleanParameter("Generate starter file trigger", "GnrStarter", "Generate starter file", GH_ParamAccess.item);
            pManager.AddBooleanParameter("Clear temporary files trigger", "ClrTemp", "Clear temporary file trigger", GH_ParamAccess.item);
            
            pManager.AddTextParameter("Working Directory", "WorkDir", "Working Directory", GH_ParamAccess.item);
            pManager.AddIntegerParameter("CheckPoint index", "CkPointI", "CheckPoint index in point list", GH_ParamAccess.list);
            pManager[13].Optional = true;
            pManager[21].Optional = true;
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddTextParameter(" ", " ", " ", GH_ParamAccess.item);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object can be used to retrieve data from input parameters and 
        /// to store data in output parameters.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
        /// physics
            bool usedecr = false;
            double coeWB = double.MinValue;
            double coeBD = double.MinValue;
            double coeBA = double.MinValue;
            double coeDA = double.MinValue;

            double Bthick = double.MinValue;
            double Dthick = double.MinValue;
            double EnvrTemper = double.MinValue;
            double WtrTemper = double.MinValue;
            double WtrFlow = double.MinValue;
            List<double> BaseMat = new List<double>();
            List<double> DecrMat = new List<double>();
            bool suc;
            suc = DA.GetData(0, ref usedecr) && DA.GetData(1, ref coeWB) && DA.GetData(2, ref coeBD) &&
                  DA.GetData(3, ref coeBA) && DA.GetData(4, ref coeDA) &&
                  DA.GetData(5, ref Bthick) && DA.GetData(6, ref Dthick) && DA.GetData(7, ref EnvrTemper) &&
                  DA.GetData(8, ref WtrTemper) && DA.GetData(9, ref WtrFlow) &&
                  DA.GetDataList(10, BaseMat) && DA.GetDataList(11, DecrMat);
            if (!suc)
            {
                return;
            }
        ///geometry
            Mesh Basemesh = new Mesh();
            List<Point3d> Decrmesh = new List<Point3d>();
            suc = DA.GetData(12, ref Basemesh);
            DA.GetDataList(13, Decrmesh);
            if (!suc)
            {
                return;
            }
        ///water
            bool IsPolyline = true;
            List<Point3d> Pts = new List<Point3d>();
            List<double> Wids = new List<double>();
            List<double> Areas = new List<double>();
            suc = DA.GetData(14, ref IsPolyline) && DA.GetDataList(15, Pts) && 
                  DA.GetDataList(16, Wids) && DA.GetDataList(17, Areas);
            if (!suc)
            {
                return;
            }
        ///triggers && directory && checkpoint
            bool GenerateTrigger = false;
            bool ClearTrigger = false;
            string Dir = string.Empty;
            List<int> Indices = new List<int>();
            suc = DA.GetData(18, ref GenerateTrigger) && DA.GetData(19, ref ClearTrigger)
               && DA.GetData(20, ref Dir);
            DA.GetDataList(21, Indices);
            if (!suc)
            {
                return;
            }
            
        //data verification
            if(BaseMat.Count != 3 || (usedecr && DecrMat.Count != 3))
            {
                DA.SetData(0, "Material format not satisfied\n");
                return;
            }
            if (Wids.Count == 0 || Pts.Count == 0 || Areas.Count == 0 || Pts.Count <= 1)
            {
                DA.SetData(0, "Water format not satisfied\n");
                return;
            }
            for(int i = 0; i < Indices.Count; i++)
            {
                if (Indices[i] + 1 > Basemesh.Vertices.Count)
                {
                    DA.SetData(0, "Checkpoint Index out of range\n");
                    return;
                }
            }
            DA.SetData(0, Directory.GetCurrentDirectory());    
        //export profile
            if (GenerateTrigger == true)
            {
                if (!Directory.Exists(Dir + @"\TempFile"))
                {
                    Directory.CreateDirectory(Dir + @"\TempFile");
                }
                using (StreamWriter sw = new StreamWriter(Dir + @"\TempFile\Simustarter"))
                {
                    if (usedecr && Decrmesh.Count != 0)
                    {
                        sw.WriteLine("UseDecorate");
                    }
                    else sw.WriteLine("ForbidDecorate");
                    sw.WriteLine("CoeWB " + coeWB.ToString());
                    if (usedecr)
                    {
                        sw.WriteLine("CoeBD " + coeBD.ToString());
                        sw.WriteLine("CoeDA " + coeDA.ToString());
                    }
                    else
                    {
                        sw.WriteLine("CoeBA " + coeBA.ToString());
                    }
                    sw.WriteLine("BaseThickness " + Bthick.ToString());
                    if (usedecr)
                    {
                        sw.WriteLine("DecrThickness " + Dthick.ToString());
                    }
                    sw.WriteLine("EnvironmentTemperature " + EnvrTemper.ToString());
                    sw.WriteLine("WaterTemperature " + WtrTemper.ToString());
                    sw.WriteLine("WaterFlow " + WtrFlow.ToString());
                    sw.WriteLine("BaseMaterial " + BaseMat[0].ToString() + " " + BaseMat[1].ToString() + " " + BaseMat[2].ToString());
                    sw.WriteLine("DecorateMaterial " + DecrMat[0].ToString() + " " + DecrMat[1].ToString() + " " + DecrMat[2].ToString());
                }
                using (StreamWriter sw = new StreamWriter(Dir + @"\TempFile\Mesh"))
                {
                    for(int i = 0; i < Basemesh.Vertices.Count; i++)
                    {
                        sw.WriteLine("v " + Basemesh.Vertices[i].X.ToString() + " " + Basemesh.Vertices[i].Y.ToString() + " " + Basemesh.Vertices[i].Z.ToString());
                    }
                    for(int i = 0; i < Basemesh.Faces.Count; i++)
                    {
                        sw.WriteLine("f " + (Basemesh.Faces[i].A + 1).ToString() + " " + (Basemesh.Faces[i].B + 1).ToString() + " " + (Basemesh.Faces[i].C + 1).ToString());
                    }
                }
                if (usedecr)
                {
                    using (StreamWriter sw = new StreamWriter(Dir + @"\TempFile\Decr"))
                    {
                        for(int i = 0; i < Decrmesh.Count; i++)
                        {
                            sw.WriteLine(Decrmesh[i].X.ToString() + " " + Decrmesh[i].Y.ToString() + " " + Decrmesh[i].Z.ToString());
                        }
                    }
                }
                using (StreamWriter sw = new StreamWriter(Dir + @"\TempFile\Water"))
                {
                    sw.WriteLine(Pts.Count.ToString());
                    if (IsPolyline)
                    {
                        sw.WriteLine("PolylineWater");
                    }
                    else
                    {
                        sw.WriteLine("AreaWater");
                    }
                    if(Wids.Count == 1)
                    {
                        sw.WriteLine("Constant " + Wids[0].ToString());
                    }
                    else
                    {
                        sw.WriteLine("Changing");
                    }
                    if(Areas.Count == 1)
                    {
                        sw.WriteLine("Constant " + Areas[0].ToString());
                    }
                    else
                    {
                        sw.WriteLine("Changing");
                    }
                    for(int i = 0; i < Pts.Count; i++)
                    {
                        sw.WriteLine(Pts[i].X.ToString() + " " + Pts[i].Y.ToString() + " " + Pts[i].Z.ToString());
                    }
                    if (Wids.Count > 1)
                    {
                        for (int i = 0; i < Wids.Count; i++)
                        {
                            sw.WriteLine(Wids[i].ToString());
                        }
                    }
                    if(Areas.Count > 1)
                    {
                        for (int i = 0; i < Areas.Count; i++)
                        {
                            sw.WriteLine(Areas[i].ToString());
                        }
                    }
                }
                if (Indices != null && Indices.Count > 0)
                {
                    using (StreamWriter sw = new StreamWriter(Dir + @"\TempFile\CheckPoints"))
                    {
                        for (int i = 0; i < Indices.Count; i++)
                        {
                            sw.WriteLine((Indices[i]+1).ToString());
                        }
                    }
                }
            }
        //clear temporary files
            if(ClearTrigger == true)
            {
                try
                {
                    if (File.Exists(Dir + @"\TempFile\Temper"))
                    {
                        File.Delete(Dir + @"\TempFile\Temper");
                    }
                    if (File.Exists(Dir + @"\TempFile\Setting"))
                    {
                        File.Delete(Dir + @"\TempFile\Setting");
                    }
                    if (File.Exists(Dir + @"\TempFile\Simustarter"))
                    {
                        File.Delete(Dir + @"\TempFile\Simustarter");
                    }
                    if (File.Exists(Dir + @"\TempFile\Mesh"))
                    {
                        File.Delete(Dir + @"\TempFile\Mesh");
                    }
                    if (File.Exists(Dir + @"\TempFile\Decr"))
                    {
                        File.Delete(Dir + @"\TempFile\Decr");
                    }
                    if (File.Exists(Dir + @"\TempFile\Water"))
                    {
                        File.Delete(Dir + @"\TempFile\Water");
                    }
                    if (File.Exists(Dir + @"\TempFile\CheckPoints"))
                    {
                        File.Delete(Dir + @"\TempFile\CheckPoints");
                    }
                    if (File.Exists(Dir + @"\TempFile\Status"))
                    {
                        File.Delete(Dir + @"\TempFile\Status");
                    }
                    if (Directory.Exists(Dir + @"\TempFile"))
                    {
                        Directory.Delete(Dir + @"\TempFile");
                    }
                }
                catch
                {
                    DA.SetData(0, "Some error occured when deleting temp files");
                    return;
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
        public override Guid ComponentGuid => new Guid("A5E2336B-A133-414D-83BE-9006A2C38C22");
    }
}