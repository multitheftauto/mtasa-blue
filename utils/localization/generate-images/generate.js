const fs = require('fs').promises;

require('chromedriver');

const {Browser, Builder} = require('selenium-webdriver');
const chrome = require('selenium-webdriver/chrome');

const driver = new Builder().forBrowser(Browser.CHROME).setChromeOptions(new chrome.Options().headless()).build();

(async () => {
    try {
        const locales = (await fs.readdir('locale', { withFileTypes: true }))
            .filter(dirent => dirent.isDirectory() && dirent.name[0] !== '.')
            .map(dirent => dirent.name);

        console.log('Doing', locales);

        await driver.get(`http://localhost/?locales=${locales.join(',')}`);

        const result = await driver.executeScript('return main()');

        const writes = result.map(([locale, pngs]) => (async () => {
            await fs.mkdir(`uncompressed/${locale}`, { recursive: true });
            await Promise.allSettled(pngs.map(([filename, base64]) => fs.writeFile(`uncompressed/${locale}/${filename}`, Buffer.from(base64, 'base64'))));

            return console.log(`${locale} done!`);
        })());

        await driver.quit();

        await Promise.allSettled(writes);

        console.log('All done!');
    } catch (e) {
        await driver.quit();

        throw e;
    }
})();
