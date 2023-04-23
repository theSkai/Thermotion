using Grasshopper;
using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.IO;

namespace ThermotionDesignTool2
{
    public class ThermotionSimulationSetter : GH_Component
    {
        /// <summary>
        /// Each implementation of GH_Component must provide a public 
        /// constructor without any arguments.
        /// Category represents the Tab in which the component will appear, 
        /// Subcategory the panel. If you use non-existing tab or panel names, 
        /// new tabs/panels will automatically be created.
        /// </summary>
        public ThermotionSimulationSetter()
          : base("SimulatorSetter", "SS",
            "Setter for Simulator",
            "Thermotion", "Simulator")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddTextParameter("Simulator Directory", "Directory", "the directory where simulation programme are stored and executed", GH_ParamAccess.item);
            pManager.AddNumberParameter("Temperature", "Temperature", "Temperature", GH_ParamAccess.item);
            pManager.AddNumberParameter("Rate of water flow", "Water Flow", "Water Flow", GH_ParamAccess.item);
            pManager.AddBooleanParameter("Water direction", "Water direction", "Water direction", GH_ParamAccess.item);
            pManager.AddBooleanParameter("Loading Air", "Air", "True for loading air", GH_ParamAccess.item);
            pManager.AddBooleanParameter("Start", "Start", "Start", GH_ParamAccess.item);
            pManager.AddBooleanParameter("Pauser", "Pauser", "Pauser", GH_ParamAccess.item);
            pManager.AddBooleanParameter("Kill", "Kill", "Kill", GH_ParamAccess.item);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddTextParameter("Output", "Output", "Output", GH_ParamAccess.item);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object can be used to retrieve data from input parameters and 
        /// to store data in output parameters.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
            string fname = string.Empty;
            double temperature = double.MinValue;
            double waterflow = double.MinValue;
            bool waterdir = true;
            bool loadingair = false;
            bool start = false;
            bool pause = false;
            bool kill = false;

            bool suc = DA.GetData(0, ref fname) && DA.GetData(1, ref temperature)
                    && DA.GetData(2, ref waterflow) && DA.GetData(3, ref waterdir)
                    && DA.GetData(4, ref loadingair) && DA.GetData(5, ref start)
                    && DA.GetData(6, ref pause) && DA.GetData(7, ref kill);
            if (!suc)
            {
                return;
            }
            else
            {
                string statusfile = fname + @"\TempFile\Status";
                string settingfile = fname + @"\TempFile\Setting";
                //check status
                int status = 0;
                if (File.Exists(statusfile))
                {
                    try
                    {
                        using (FileStream fs = new FileStream(statusfile, FileMode.Open, FileAccess.Read))
                        {
                            using(StreamReader sr = new StreamReader(fs))
                            {
                                string line = sr.ReadLine();
                                status = int.Parse(line);
                            }
                        }
                    }
                    catch
                    {
                        status = -1;
                    }
                }
                else
                {
                    status = -1;
                }
                //status: -1 not begin, 1, begin, 2, disabled
                if(status == -1)
                {
                    if(start == true)
                    {
                        //check if there is initialization file
                        string checkstarter = fname + @"\TempFile\Simustarter";
                        if (!File.Exists(checkstarter))
                        {
                            DA.SetData(0, "You haven't update current configuration yet");
                            return;
                        }
                        else
                        {
                            //create statusfile
                            using (FileStream fs = new FileStream(statusfile, FileMode.Create, FileAccess.Write))
                            {
                                using (StreamWriter sw = new StreamWriter(fs))
                                {
                                    sw.WriteLine("1");
                                }
                            }
                            //create setting
                            using (FileStream fs = new FileStream(settingfile, FileMode.Create, FileAccess.Write, FileShare.ReadWrite))
                            {
                                using (StreamWriter sw = new StreamWriter(fs))
                                {
                                    if (waterflow == 0)
                                    {
                                        sw.WriteLine("Pause");
                                    }
                                    else
                                    {
                                        sw.WriteLine("Going");
                                    }

                                    if (waterdir)
                                    {
                                        sw.WriteLine("Forward");
                                    }
                                    else
                                    {
                                        sw.WriteLine("Backward");
                                    }

                                    if (loadingair)
                                    {
                                        sw.WriteLine("Air");
                                    }
                                    else
                                    {
                                        sw.WriteLine("Water");
                                    }

                                    sw.WriteLine("Temperature " + temperature.ToString());
                                    sw.WriteLine("Waterflow " + waterflow.ToString());
                                    sw.Flush();
                                }
                            }
                            //start program
                            System.Diagnostics.Process process = new System.Diagnostics.Process();
                            process.StartInfo.CreateNoWindow = true;
                            process.StartInfo.FileName = "wsl";
                            process.StartInfo.UseShellExecute = false;
                            process.StartInfo.WorkingDirectory = fname;
                            process.StartInfo.Arguments = "./SIMULATOR";
                            process.Start();
                        }
                    }
                    else return;
                }
                if(status == 1)
                {
                    if (start == true)
                    { 
                        return;
                    }
                    else if (pause == true)
                    {
                        //disable project
                        using (FileStream fs = new FileStream(settingfile, FileMode.Create, FileAccess.Write, FileShare.ReadWrite))
                        {
                            using (StreamWriter sw = new StreamWriter(fs))
                            {
                                sw.WriteLine("Disable");
                            }
                        }
                        //change status
                        using (FileStream fs = new FileStream(statusfile, FileMode.Create, FileAccess.Write))
                        {
                            using (StreamWriter sw = new StreamWriter(fs))
                            {
                                sw.WriteLine("2");
                            }
                        }
                    }
                    else if (kill == true)
                    {
                        File.Delete(statusfile);
                        using (FileStream fs = new FileStream(settingfile, FileMode.Create, FileAccess.Write, FileShare.ReadWrite))
                        {
                            using (StreamWriter sw = new StreamWriter(fs))
                            {
                                sw.WriteLine("Kill");
                            }
                        }
                    }
                    else
                    {
                        //reset setting
                        using (FileStream fs = new FileStream(settingfile, FileMode.Create, FileAccess.Write, FileShare.ReadWrite))
                        {
                            using (StreamWriter sw = new StreamWriter(fs))
                            {
                                if (waterflow == 0)
                                {
                                    sw.WriteLine("Pause");
                                }
                                else
                                {
                                    sw.WriteLine("Going");
                                }

                                if (waterdir)
                                {
                                    sw.WriteLine("Forward");
                                }
                                else
                                {
                                    sw.WriteLine("Backward");
                                }

                                if (loadingair)
                                {
                                    sw.WriteLine("Air");
                                }
                                else
                                {
                                    sw.WriteLine("Water");
                                }

                                sw.WriteLine("Temperature " + temperature.ToString());
                                sw.WriteLine("Waterflow " + waterflow.ToString());
                                sw.Flush();
                            }
                        }
                    }
                }
                if(status == 2)
                {
                    if (start == true)
                    {
                        using (FileStream fs = new FileStream(settingfile, FileMode.Create, FileAccess.Write, FileShare.ReadWrite))
                        {
                            using (StreamWriter sw = new StreamWriter(fs))
                            {
                                if (waterflow == 0)
                                {
                                    sw.WriteLine("Pause");
                                }
                                else
                                {
                                    sw.WriteLine("Going");
                                }

                                if (waterdir)
                                {
                                    sw.WriteLine("Forward");
                                }
                                else
                                {
                                    sw.WriteLine("Backward");
                                }

                                if (loadingair)
                                {
                                    sw.WriteLine("Air");
                                }
                                else
                                {
                                    sw.WriteLine("Water");
                                }

                                sw.WriteLine("Temperature " + temperature.ToString());
                                sw.WriteLine("Waterflow " + waterflow.ToString());
                                sw.Flush();
                            }
                        }
                        using (FileStream fs = new FileStream(statusfile, FileMode.Create, FileAccess.Write))
                        {
                            using (StreamWriter sw = new StreamWriter(fs))
                            {
                                sw.WriteLine("1");
                            }
                        }
                    }
                    else if (kill == true)
                    {
                        File.Delete(statusfile);
                        using (FileStream fs = new FileStream(settingfile, FileMode.Create, FileAccess.Write, FileShare.ReadWrite))
                        {
                            using (StreamWriter sw = new StreamWriter(fs))
                            {
                                sw.WriteLine("Kill");
                            }
                        }
                    }
                    else return;
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
        public override Guid ComponentGuid => new Guid("B0168724-1BEF-4EAE-989A-BB73C64F2610");
    }
}