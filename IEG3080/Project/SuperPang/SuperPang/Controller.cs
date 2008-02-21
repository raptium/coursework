using System;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    public class Controller
    {
        private Model model = null;
        public Controller(Model model)
        {
            this.model = model;
        }
        // for the key up event
        public void keyHandler(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            String inputKey = e.KeyCode.ToString();
        }
    }

}
