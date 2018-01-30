using CortexAccess;
using System;
using System.Threading;

namespace RecordData
{
    class Program
    {
        const string Username = "your_username";
        const string Password = "your_password";
        const string LicenseId = "your_license";
        const int DebitNumber = 2; // default number of debit

        static void Main(string[] args)
        {
            Console.WriteLine("RECORD DATA DEMO");
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
                p.Authorize(LicenseId, DebitNumber);
                Thread.Sleep(5000); //wait for authorize
            }

            if (!String.IsNullOrEmpty(p.GetSelectedHeadsetId()) && !String.IsNullOrEmpty(p.GetAccessToken()))
            {
                // Create Sesssion
                if(!p.IsCreateSession)
                {
                    p.CreateSession();
                    Thread.Sleep(5000); //wait for creating session
                }

                if (p.IsCreateSession)
                {
                    Console.WriteLine("Session have created successfully");
                }

            }
            Console.WriteLine("Press S to start");
            Console.WriteLine("Press N to start a new Record");
            Console.WriteLine("Press A,B,C to inject marker");
            Console.WriteLine("Press E to end of a record (stop record)");
            Console.WriteLine("Press Q to querry session and quit");

            while (true) {

                int key = (int)Console.ReadKey().Key;

                if (key == (int)ConsoleKey.E)
                {
                    p.StopRecord();
                    Thread.Sleep(5000);
                }
                else if (key == (int)ConsoleKey.S) // start record
                {
                    p.StartRecord("300118_1", "test4", "helloworld");
                    Thread.Sleep(5000);
                }
                else if (key == (int)ConsoleKey.N) // next start record
                {
                    p.StartRecord("300118_2", "test5", "helloyou");
                    Thread.Sleep(5000);
                }
                else if(key == (int)ConsoleKey.A)
                {
                    // Inject marker
                    p.InjectMarker("A", 10,Utils.GetEpochTimeNow());

                }
                else if (key == (int)ConsoleKey.B)
                {
                    // Inject marker
                    p.InjectMarker("B", 11, Utils.GetEpochTimeNow());

                }
                else if (key == (int)ConsoleKey.C)
                {
                    // Inject marker
                    p.InjectMarker("C", 12, Utils.GetEpochTimeNow());

                }
                else if (key == (int)ConsoleKey.Q)
                {
                    // Querry Sessions before quit
                    p.QuerySession();
                    Thread.Sleep(5000);
                    break;

                }
            }
            Console.WriteLine("End of Program");

        }
    }
}
