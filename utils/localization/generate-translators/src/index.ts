import crowdin from '@crowdin/crowdin-api-client';
import { promises as fs, existsSync as fsExistsSync } from 'node:fs';
import yargs from 'yargs';
import { hideBin } from 'yargs/helpers';

interface User {
    id: string,
    username: string,
    fullName: string,
    avatarUrl: string,
    joined: string,
}

interface Language {
    id: string,
    name: string,
}

interface Member {
    user: User,
    languages: Array<Language>,
    translated: number,
    target: number,
    approved: number,
    voted: number,
    positiveVotes: number,
    negativeVotes: number,
    winning: number,
}

type StringArrayMap = {
    [key: string]: string[];
};

// Define command line arguments
const argv = yargs(hideBin(process.argv))
    .env('CROWDIN')
    .option('token', {
        alias: 't',
        type: 'string',
        description: 'The Crowdin API token',
        demandOption: true,
    })
    .option('organization', {
        alias: 'o',
        type: 'string',
        description: 'Name of the organization on Crowdin',
        demandOption: true,
    })
    .option('projectId', {
        alias: 'p',
        type: 'number',
        description: 'Numerical ID of the project on Crowdin',
        demandOption: true,
    })
    .option('ignoreUserId', {
        alias: 'i',
        type: 'string',
        description: 'ID of the user to ignore in the report',
    })
    .help()
    .parseSync();

function delay(ms: number) {
    return new Promise((resolve) => setTimeout(resolve, ms));
};

async function generateTopMembersReport(): Promise<Array<Member>> {
    const reportsApi = new crowdin.Reports({
        token: argv.token,
        organization: argv.organization,
    });

    let report: Awaited<ReturnType<typeof reportsApi.generateReport>>;
    try {
        report = await reportsApi.generateReport(argv.projectId, {
            name: 'top-members',
            schema: {
                format: 'json'
            }
        });
    }
    catch (e) {
        console.error('Failed to generate report:', e);
        process.exit(1);
    }

    await delay(3000);

    while (true) {
        try {
            const reportStatus = await reportsApi.checkReportStatus(argv.projectId, report.data.identifier);
            console.log(`Report download progress: ${reportStatus.data.progress}%`);

            if (reportStatus.data.status === 'finished') {
                const download = await reportsApi.downloadReport(argv.projectId, report.data.identifier);
                const response = await fetch(download.data.url);
                const json = await response.json();
                return json.data;
            }
        }
        catch (e) {
            console.error('Failed to download report:', e);
            process.exit(2);
        }

        await delay(1000);
    }
}

async function main() {
    // Check if crowdin-report.json exists and load it, to avoid generating the report again.
    // If it doesn't exist, generate the report and save it to crowdin-report.json.
    let topMembers: Array<Member>;

    if (fsExistsSync('crowdin-report.json')) {
        topMembers = JSON.parse(await fs.readFile('crowdin-report.json', 'utf-8'));
    } else {
        topMembers = await generateTopMembersReport();
        await fs.writeFile('crowdin-report.json', JSON.stringify(topMembers, null, 2));
    }

    // Filter out the organization user from the top members list.
    if (argv.ignoreUserId) {
        topMembers = topMembers.filter(member => member.user.id !== argv.ignoreUserId);
    }

    // Filter out members without winning translations.
    topMembers = topMembers.filter(member => member.winning > 0);
    
    // Map members to the languages they contributed to.
    const membersByLanguage = new Map<string, Array<string>>();

    for (const member of topMembers) {
        for (const language of member.languages) {
            const languageId = language.id.replace('-', '_');
            let members = membersByLanguage.get(languageId);

            if (!members) {
                members = [];
                membersByLanguage.set(languageId, members);
            }

            const fullName = member.user.fullName.replaceAll(/\s+/g, " ").trim();
            members.push(fullName);
        }
    }

    // Include the previous contributors from Pootle in the top members list.
    if (fsExistsSync('pootle-translators.json')) {
        const pootle: StringArrayMap = JSON.parse(await fs.readFile('pootle-translators.json', 'utf-8'));

        for (const [language1, translators] of Object.entries(pootle)) {
            const language2 = language1.split('_')[0];
            const language3 = language1.replace('_', '-');

            if (membersByLanguage.has(language1)) {
                membersByLanguage.get(language1)!.push(...translators);
            }
            else if (membersByLanguage.has(language2)) {
                membersByLanguage.get(language2)!.push(...translators);
            }
            else if (membersByLanguage.has(language3)) {
                membersByLanguage.get(language3)!.push(...translators);
            }
            else {
                console.log(`[!] Locale has no translators on Crowdin, only on Pootle (1:${language1}, 2:${language2}, 3:${language3})`);
                membersByLanguage.set(language1, translators);
            }
        }
    }

    // Filter out duplicate members from each group.
    for (const [language, members] of membersByLanguage.entries()) {
        const uniqueMembers = new Set(members);
        membersByLanguage.set(language, Array.from(uniqueMembers));
    }

    // Sort the members by name in ascending order.
    for (const members of membersByLanguage.values()) {
        members.sort((a, b) => a.localeCompare(b, undefined, { sensitivity: 'base' }));
    }

    // For debugging purposes:
    // console.log('Top members:', membersByLanguage);

    // Convert the language map to an array and sort it by language name.
    const ascending = Array.from(membersByLanguage.entries()).sort(([a], [b]) => a.localeCompare(b));
    const sorted = Object.fromEntries(ascending);
    await fs.writeFile('crowdin-translators.json', JSON.stringify(sorted, null, 2));

    console.log('Done.');
}

main().catch(console.error);
