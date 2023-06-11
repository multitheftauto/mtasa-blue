/**
 * Step 1
 * Spawn a temporary HTTP server
 */
import { createServer } from 'http-server';

const httpServer = createServer();

await new Promise((resolve) => httpServer.listen(8080, resolve));

/**
 * Step 2
 * Copy MTA/locale folder over here
 */
import fse from 'fs-extra';

try {
	fse.copySync('../../../Shared/data/MTA San Andreas/MTA/locale', 'locale', { overwrite: true });
} catch (err) {
	console.error(err);

	httpServer.close();

	process.exit(1);
}

/**
 * Step 3
 * Generate images
 */
import fs from 'node:fs/promises';

import 'chromedriver';

import {Browser, Builder} from 'selenium-webdriver';
import chrome from 'selenium-webdriver/chrome.js';

const driver = new Builder().forBrowser(Browser.CHROME).setChromeOptions(new chrome.Options().headless()).build();

await (async () => {
    try {
        const locales = (await fs.readdir('locale', { withFileTypes: true }))
            .filter(dirent => dirent.isDirectory() && dirent.name[0] !== '.')
            .map(dirent => dirent.name);

        console.log('Doing', locales);

        await driver.get(`http://localhost:8080/?locales=${locales.join(',')}`);

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

httpServer.close();

fse.rmdirSync('locale', { recursive: true, force: true });

/**
 * Step 4
 * Compress images
 */
await (async () => {
    const { ImagePool } = (await import('@squoosh/lib')).default;
    const { cpus } = (await import('os')).default;
    const imagePool = new ImagePool(cpus().length);

    const locales = (await fs.readdir('uncompressed', { withFileTypes: true }))
        .filter(dirent => dirent.isDirectory() && dirent.name[0] !== '.')
        .map(dirent => dirent.name);

    console.log('Doing', locales);

    await Promise.allSettled(locales.map(async (locale) => {
        console.log(`Doing ${locale}...`);

        const pngs = (await fs.readdir(`uncompressed/${locale}`, { withFileTypes: true }))
            .filter(dirent => dirent.isFile() && dirent.name[0] !== '.' && dirent.name.endsWith('.png'))
            .map(dirent => dirent.name);

        await Promise.allSettled(pngs.map(async (filename) => {
            const image = imagePool.ingestImage(`uncompressed/${locale}/${filename}`);

            try {
                await image.preprocess({ quant: { dither: 0.1, numColors: 8 } });
            } catch (e) {
                console.error('preprocess', `uncompressed/${locale}/${filename}`, e);
            }

            try {
                await image.encode({ oxipng: { level: 6 } });
            } catch (e) {
                console.error('encode', `uncompressed/${locale}/${filename}`, e);
            }

            return fs.writeFile(`../../../Shared/data/MTA San Andreas/MTA/locale/${locale}/${filename}`, (await image.encodedWith.oxipng).binary);
        }));

        console.log(`${locale} done!`);
    }));

    await imagePool.close();

    console.log('All done!');
})();

fse.rmdirSync('uncompressed', { recursive: true, force: true });
