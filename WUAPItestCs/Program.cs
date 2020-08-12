using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using WUApiLib;

namespace WUAPItestCs
{
    class Program
    {
        private const string RebootFlagFileName = "C:\\Windows\\reboot.required";

        static int Main(string[] args)
        {
            try {
                return InstallUpdates();
            } catch (COMException e) {
                Console.Error.WriteLine(e.Message);
                Console.Error.WriteLine(e.StackTrace);
                return (int)BetterWin32Errors.Win32Error.ERROR_INSTALL_FAILURE;
            }
        }

        static int InstallUpdates()
        {
            UpdateSession session = new UpdateSession();
            session.ClientApplicationID = "WUAPItestCs";
            IUpdateSearcher searcher = session.CreateUpdateSearcher();

            Console.WriteLine("Searching for updates ...");

            ISearchResult searchResult = searcher.Search("IsInstalled=0 and Type='Software' and IsHidden=0");
            UpdateCollection found = searchResult.Updates;

            if (found.Count == 0) {
                Console.WriteLine("No applicable updates found.");
                return 0;
            }

            Console.WriteLine("List of applicable updates:");

            UpdateCollection selected = new UpdateCollection();
            foreach (IUpdate update in found)
            {
                Console.WriteLine(update.Title);

                if (update.InstallationBehavior.CanRequestUserInput) {
                    Console.WriteLine("    skipped because it requires user input");
                    continue;
                } else if (!update.EulaAccepted) {
                    Console.WriteLine("    skipped because EULA declined");
                    continue;
                } else if (update.Title.StartsWith("Feature update")) {
                    Console.WriteLine("    skipped because feature updates are interactive");
                    continue;
                }

                selected.Add(update);
            }

            if (selected.Count == 0) {
                Console.WriteLine("All applicable updates are skipped");
                return 0;
            }

            Console.WriteLine("Downloading updates ...");

            UpdateDownloader downloader = session.CreateUpdateDownloader();
            downloader.Updates = selected;
            downloader.Download();

            bool reboot = false;

            Console.WriteLine("Downloaded updates:");

            UpdateCollection downloaded = new UpdateCollection();

            foreach (IUpdate update in selected) {
                if (update.IsDownloaded) {
                    Console.WriteLine(update.Title);
                    downloaded.Add(update);
                }

                reboot |= (update.InstallationBehavior.RebootBehavior != InstallationRebootBehavior.irbNeverReboots);
            }

            if (downloaded.Count == 0) {
                Console.Error.WriteLine("No updates were successfully downloaded.");
                return 1;
            }

            if (reboot) {
                Console.WriteLine("These updates may require a reboot.");
            }

            Console.WriteLine("Installing updates ...");

            IUpdateInstaller installer = session.CreateUpdateInstaller();
            installer.Updates = downloaded;

            IInstallationResult installationResult = installer.Install();
            OperationResultCode operationResultCode = installationResult.ResultCode;
            Console.WriteLine("Installation result: " + operationResultCode.ToString());

            if (operationResultCode == OperationResultCode.orcAborted
                   || operationResultCode == OperationResultCode.orcFailed) {
                Console.Error.WriteLine("Installation aborted or failed.");
                return 1;
            }

            if (installationResult.RebootRequired) {
                Console.WriteLine("Installed updates require reboot.");

                string pdqServer = Environment.GetEnvironmentVariable("PDQSERVER");
                pdqServer = pdqServer?.Split('.')[0];

                if (pdqServer != null
                        && String.Compare(pdqServer, Environment.MachineName,
                            StringComparison.InvariantCultureIgnoreCase) == 0) {
                    File.Create(RebootFlagFileName);
                } else {
                    Console.WriteLine("Skipping required reboot because this is " +
                                      "the PDQ Deploy Server.");
                    return (int)BetterWin32Errors.Win32Error.ERROR_SUCCESS_REBOOT_REQUIRED;
                }
            }

            return 0;
        }
    }
}
