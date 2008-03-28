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
        private IEG3080.View currentView = null;

        public MainForm()
        {
            InitializeComponent();
            this.Icon = Properties.Resources.MacPro;
            this.BackgroundImage = Properties.Resources.backgroud;
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
                currentModel = new Model();

                // create the controller
                currentModel.createController();

                // create the view 
                currentModel.createView();
            }
            else
                // switch the view in real time
                currentModel.createView();
        }

        private void menuAbout_Click(object sender, EventArgs e)
        {
            AboutBox box = new AboutBox();
            box.ShowDialog();
        }

    }
}