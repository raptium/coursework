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
        public void keyUpHandler(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            String inputKey = e.KeyCode.ToString();
            if(inputKey.Equals("Left") || inputKey.Equals("Right"))
                model.stopHero();
            
        }

        // for the key press event
        public void keyDownHandler(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            String inputKey = e.KeyCode.ToString();

            if (inputKey.Equals("Left"))
                model.moveHero(-1);
            else if (inputKey.Equals("Right"))
                model.moveHero(1);
            else if (inputKey.Equals("Down"))
                model.stopHero();
            else if (inputKey.Equals("Space"))
                model.getHero().jump();
        }
    }

}
