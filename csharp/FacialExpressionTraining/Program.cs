using CortexAccess;
using System;
using System.Threading;

namespace FacialExpressionTraining
{
    class Program
    {
        const string Username = "your_username";
        const string Password = "your_password";

        static void Main(string[] args)
        {
            Console.WriteLine("FACIAL EXPRESSION TRAINING");
            Console.WriteLine("Please wear Headset with good signal!!!");
            Process p = new Process();
            Thread.Sleep(5000); //wait for querrying user login
            if (String.IsNullOrEmpty(p.GetUserLogin()))
            {
                p.Login(Username, Password);
                Thread.Sleep(1000); //wait for login
            }

            // Show username login
            Console.WriteLine("Username :" + p.GetUserLogin());

            if (p.AccessCtr.IsLogin)
            {
                // Send Authorize
                p.Authorize();
                Thread.Sleep(5000); //wait for authorize
            }

            // get Detection Information
            //p.QuerryDetectionInfo("facialExpression");
            //Thread.Sleep(2000); //wait for get detection information

            if (!String.IsNullOrEmpty(p.GetSelectedHeadsetId()) && !String.IsNullOrEmpty(p.GetAccessToken()))
            {
                // Create Sesssion
                p.CreateSession();
                Thread.Sleep(5000); //wait for creating session

                if (p.IsCreateSession)
                {
                    Console.WriteLine("Session have created successfully");
                    // Subcribe sys event
                    p.SubcribeData("sys");
                    Thread.Sleep(5000);
                }
            }
            // Create / load a profile
            Console.WriteLine("Create a profile");
            p.LoadProfile("FE_30_1_18_2"); // Load an existed profile or create a new Profile
            Thread.Sleep(2000);
            // Training neutral
            Console.WriteLine("\n###### Train NEUTRAL Action");
            p.StartFE("neutral");
            Thread.Sleep(10000);
            p.AcceptFE();
            Thread.Sleep(2000);

            // Training blink
            //p.StartFE("blink"); // Currently, can not train blink action
            //Thread.Sleep(10000);
            //p.AcceptFE();
            //Thread.Sleep(2000);

            // Training smile
            Console.WriteLine("\n###### Train SMILE Action");
            p.StartFE("smile");
            Thread.Sleep(10000);
            p.AcceptFE();
            Thread.Sleep(2000);

            // Save profile
            p.SaveProfile();
            Thread.Sleep(5000);

            // Upload Profile
            //p.UploadProfile();
            //Thread.Sleep(3000);

            // Subcribe fac event -> show training result
            p.SubcribeData("fac");
            Thread.Sleep(5000);
        }
    }
}
