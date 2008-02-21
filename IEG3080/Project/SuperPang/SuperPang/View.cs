using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    public abstract class View : System.Windows.Forms.Panel
    {
        protected Model model = null;
        protected Controller controller = null;
        protected Graphics g = null;

        public View(Model model, Controller controller) : base()
        {
            this.model = model;
            this.controller = controller;

            model.ModelUpdateEvent += new Model.ViewUpdater(this.update);
            this.Height = 690;
            this.Width = 1024;

            g = this.CreateGraphics();

        }
        // common interface for receiving message from model
        public abstract void update(Model model);
    }
}
