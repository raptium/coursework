using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SuperPang
{
    public partial class MainForm : Form
    {

        //declare view and model in Form application.
        private Model currentModel = null;
        private Panel currentView = null;
        //add the view to Form
        //this.Controls.Add(currentView);

        public MainForm()
        {
            InitializeComponent();
        }

        private void menuExit_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void menuStart_Click(object sender, EventArgs e)
        {
            if (currentModel == null)
            {
                // create the model of the game
                currentModel = new Model(this);

                // create the controller
                currentModel.createController();

                // create the view 
                currentModel.createView();
            }
            else
                // switch the view in real time
                currentModel.createView();
        }

        public void switchView(Panel currentView)
        {
            if (this.currentView != null)
                this.Controls.Remove(this.currentView);

            this.Controls.Add(currentView);
            this.currentView = currentView;
        }
    }
}