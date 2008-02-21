using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    class GameView : View
    {
        public GameView(Model model, Controller controller)
            : base(model, controller)
        {

        }

        public override void update(Model model)
        {
            
            makeHero(model.getHero());
            Invalidate();
        }

        private void makeHero(Hero hero){
            Bitmap img = Properties.Resources.Hero;


            g.DrawImage(img, hero.getLocationInfo().X, hero.getLocationInfo().Y);
        }
    }
}
