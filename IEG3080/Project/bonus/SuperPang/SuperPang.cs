using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace SuperPang
{
    static class SuperPang
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        ///[STAThread]
        static void Main()
        {
            //Application.EnableVisualStyles();
            //Application.SetCompatibleTextRenderingDefault(false);
            new Model();
        }
    }
}