using System;
using System.IO;
using System.Runtime.InteropServices;
using BetterWin32Errors;
using WUApiLib;

namespace WUAPItestCs
{
    internal class Program
    {
        private const string RebootFlagFileName = "C:\\Windows\\reboot.required";

        private static int Main(string[] args)
        {
            try {
                return InstallUpdates();
            } catch (COMException e) {
                Console.Error.WriteLine(e.Message);
                Console.Error.WriteLine(e.StackTrace);
                return (int)Win32Error.ERROR_INSTALL_FAILURE;
            }
        }

        private static int InstallUpdates()
        {
            var session = new UpdateSession();
            session.ClientApplicationID = "WUAPItestCs";
            var searcher = session.CreateUpdateSearcher();

            Console.WriteLine("Searching for updates ...");

            var searchResult = searcher.Search("IsInstalled=0 and Type='Software' and IsHidden=0");
            var found = searchResult.Updates;

            if (found.Count == 0) {
                Console.WriteLine("No applicable updates found.");
                return (int)BetterWin32Errors.Win32Error.ERROR_SUCCESS;
            }

            Console.WriteLine("List of applicable updates:");

            var selected = new UpdateCollection();
            foreach (IUpdate update in found) {
                Console.WriteLine(update.Title);

                if (update.InstallationBehavior.CanRequestUserInput) {
                    Console.WriteLine("    skipped because it requires user input");
                    continue;
                }

                if (!update.EulaAccepted) {
                    Console.WriteLine("    skipped because EULA declined");
                    continue;
                }

                if (update.Title.StartsWith("Feature update")) {
                    Console.WriteLine("    skipped because feature updates are interactive");
                    continue;
                }

                selected.Add(update);
            }

            if (selected.Count == 0) {
                Console.WriteLine("All applicable updates are skipped");
                return (int)BetterWin32Errors.Win32Error.ERROR_SUCCESS;
            }

            Console.WriteLine("Downloading updates ...");

            var downloader = session.CreateUpdateDownloader();
            downloader.Updates = selected;
            downloader.Download();

            var reboot = false;

            Console.WriteLine("Downloaded updates:");

            var downloaded = new UpdateCollection();

            foreach (IUpdate update in selected) {
                if (update.IsDownloaded) {
                    Console.WriteLine(update.Title);
                    downloaded.Add(update);
                }

                reboot |= update.InstallationBehavior.RebootBehavior != InstallationRebootBehavior.irbNeverReboots;
            }

            if (downloaded.Count == 0) {
                Console.Error.WriteLine("No updates were successfully downloaded.");
                return 1;
            }

            if (reboot) Console.WriteLine("These updates may require a reboot.");

            Console.WriteLine("Installing updates ...");

            var installer = session.CreateUpdateInstaller();
            installer.Updates = downloaded;

            var installationResult = installer.Install();
            var operationResultCode = installationResult.ResultCode;
            Console.WriteLine("Installation result: " + operationResultCode);

            if (operationResultCode == OperationResultCode.orcAborted
                || operationResultCode == OperationResultCode.orcFailed) {
                Console.Error.WriteLine("Installation aborted or failed.");
                return (int)BetterWin32Errors.Win32Error.ERROR_INSTALL_FAILED;
            }

            if (installationResult.RebootRequired) {
                Console.WriteLine("Installed updates require reboot.");

                var pdqServer = Environment.GetEnvironmentVariable("PDQSERVER");
                pdqServer = pdqServer?.Split('.')[0];

                if (pdqServer == null) {
                    Console.WriteLine("Not running from PDQ Deploy; skipping required reboot.");
                    return (int)Win32Error.ERROR_SUCCESS_REBOOT_REQUIRED;
                }

                if (string.Compare(pdqServer, Environment.MachineName,
                    StringComparison.InvariantCultureIgnoreCase) == 0) {
                    Console.WriteLine("Skipping required reboot because this is " +
                                      "the PDQ Deploy Server.");
                    return (int)Win32Error.ERROR_SUCCESS_REBOOT_REQUIRED;
                }

                File.Create(RebootFlagFileName);
            }

            return (int)BetterWin32Errors.Win32Error.ERROR_SUCCESS;
        }
    }
}
