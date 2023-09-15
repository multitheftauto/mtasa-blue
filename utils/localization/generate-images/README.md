# generate-images

This tool can be used to automatically generate compressed main menu images for all locales.

## Usage

1. Run `git checkout l10n/master` to switch to the current Crowdin sync branch so you're working with the latest pending locales (make sure the branch is up to date with `git pull`)
2. Run `cd utils/localization/generate-images`
3. Run `npm ci` to install npm dependencies
4. Run `npm start` to start the generator
5. Run `git status` to confirm that all updated images have now been generated in the `/Shared/data/MTA San Andreas/MTA/locale` directory
6. Commit the updated image files into the `l10n/master` branch
