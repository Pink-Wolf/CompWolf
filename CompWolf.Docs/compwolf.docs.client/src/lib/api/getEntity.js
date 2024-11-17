import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

const DATABASE_URL = `https://localhost:7112/`
const EXAMPLE_URL = `${DATABASE_URL}examples/`
const API_DOCS_URL = `${DATABASE_URL}api/docs/`
const API_SOURCE_URL = `${DATABASE_URL}api/source/`

async function getJson(path) {
    const response = await fetch(path, {
        method: 'GET',
        cache: `no-cache`,
        headers: {
            'Content-Type': 'application/json',
        },
    })
    if (!response.ok) throw new Error(`Could not get data from ${path} (${response.status}):\n${await response.text()}\n`)
    return await response.json()
}
async function postJson(path, data) {
    const response = await fetch(path, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(data, null, 2),
    })
    if (!response.ok) throw new Error(`Could not post data to ${path} (${response.status}):\n${await response.text() }\n`)
    return await response.json()
}

export async function getRawSource() {
    return await getJson(API_SOURCE_URL);
}
export async function getOverview() {
    let data = await getRawSource()
    for (let projectI = 0; projectI < data.projects.length; ++projectI) {
        const project = data.projects[projectI]
        project.project = project

        for (let headerI = 0; headerI < project.headers.length; ++headerI) {
            const header = project.headers[headerI]
            header.project = project
            header.header = header
            if (header.descriptions.length === 0)
                header.descriptions = [""]

            for (let entityI = 0; entityI < header.entities.length; ++entityI) {
                const entity = header.entities[entityI]

                function formatEntity(entity, owners) {
                    entity.project = project
                    entity.header = header
                    entity.owners = owners
                    if (entity.descriptions.length === 0)
                        entity.descriptions = [""]

                    const memberGroups = Object.values(entity.members).flat()
                    for (let nestedI = 0; nestedI < memberGroups.length; ++nestedI) {
                        const nested = memberGroups[nestedI]
                        formatEntity(nested, [...owners, entity])
                    }
                }

                formatEntity(entity, [])
            }
        }
    }
    return data
}

export async function getProject(projectName) {
    return (await getOverview())
        .projects
        .find(x => x.name === projectName)
}
export async function getHeader(projectName, headerName) {
    return (await getProject(projectName))
        .headers
        .find(x => x.name === headerName)
}
export async function getEntity(projectName, headerName, entityName) {
    const data = (await getHeader(projectName, headerName))
        .entities
        .find(x => x.name === entityName)

    if (typeof (data.members) !== typeof (undefined) && typeof (data.members[""]) !== typeof (undefined))
        data.members = { "": [], ...data.members }

    return data
}

const stdPath = `https://duckduckgo.com/?sites=cppreference.com&q=`
export async function getPathTo(name) {
    if (name.startsWith("std::"))
        return stdPath + name.substring(5)

    const overview = await getOverview()

    let memberSplitterIndex = name.indexOf("::")
    let isMember = memberSplitterIndex >= 0
    let memberName = !isMember ? "" : name.substring(memberSplitterIndex + 2, name.length).split("::").map(x => betterEncodeURIComponent(x)).join("/")
    if (isMember) {
        name = name.substring(0, memberSplitterIndex)
    }

    var path = undefined
    overview.projects?.find(project => {
        if (project.name === name) path = `/api/${project.name}/`
        project.headers?.find(header => {
            if (header.name === name) path = `/api/${project.name}/${header.name}/`
            header.entities?.find(entity => {
                if (entity.name === name) path = `/api/${project.name}/${header.name}/${betterEncodeURIComponent(entity.name)}/${memberName}`

                return (path !== undefined)
            })
            return (path !== undefined)
        })
        return (path !== undefined)
    })

    if (path === undefined) {
        console.log(`Could not find path to ${name}`)
        return undefined
    }

    return path
}
export function getPathToObject(target) {
    const pathArray = [`api`]

    if (target.project !== target) {
        pathArray.push(target.project.name)

        if (target.header !== target) {
            pathArray.push(target.header.name)

            for (var i = 0; i < target.owners.length; i++) {
                pathArray.push(target.owners[i].name)
            }
        }
    }

    pathArray.push(target.name)

    return '/' + pathArray.map(betterEncodeURIComponent).join('/')
}
export function getFullNameOfObject(target) {
    if (target.project === target) return target.name
    if (target.header === target) return target.name
    return [...target.owners.map(x => x.name), target.name].join(`::`)
}
export function getShortNameOfObject(target) {
    if (target.name == target.owners[target.owners.length - 1].name)
        return `[constructor]`
    return target.name
}
