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
    if (!response.ok) throw new Error(`Could not get data from ${path}\n${response.status}: ${response.statusText}`)
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
    if (!response.ok) throw new Error(`Could not post data to ${path}\n${response.status}: ${response.statusText}`)
    return await response.json()
}

export async function getExample(name) {
    const example = await getJson(`${EXAMPLE_URL}${name}`)
    return example
}

export async function getRawSource() {
    return await getJson(API_SOURCE_URL);
}

export async function getRawProject(name) {
    return (await getJson(`${API_DOCS_URL}${name}`))
        ?? {}
}
export async function getProject(name) {
    var source = getRawSource()
        .projects.find(x => x.name == name);
    var docs = await getRawProject(name)
    return {
        ...source,
        ...docs
    }
}

export async function getRawHeader(projectName, name) {
    return (await getJson(`${API_DOCS_URL}${projectName}/${name}`))
        ?? {}
}
export async function getHeader(projectName, name) {
    var source = getRawSource()
        .projects.find(x => x.name == projectName)
        .headers.find(x => x.name == name);
    var docs = await getRawHeader(projectName, name)
    return {
        ...source,
        ...docs,
        project: projectName,
    }
}

export async function getRawEntity(projectName, headerName, name) {
    return (await getJson(`${API_DOCS_URL}${projectName}/${headerName}/${name}`))
        ?? {}
}
export async function getEntity(projectName, headerName, name) {
    const source = getRawSource()
        .projects.find(x => x.name == projectName)
        .headers.find(x => x.name == headerName)
        .entities.find(x => x.name == name);
    const docs = await getRawEntity(projectName, headerName, name)

    var members = source.members
    for (const [groupName, groupValue] of Object.entries(docs.members)) {
        var sourceGroup = members[groupName]
        for (const member of groupValue) {
            var memberIndex = sourceGroup.findIndex(x => x.name == member.name)
            sourceGroup[memberIndex] = {
                ...sourceGroup[memberIndex],
                ...member
            }
        }
    }

    return {
        ...source,
        ...docs,
        members: members,
        project: projectName,
        header: headerName,
    }
}

export async function formatRawEntity(data) {
    const returnVal = {
        ...data,
        example: (data.example === undefined || data.example.code == "") ? undefined : await getExample(data.example)
    }
    switch (returnVal.type) {
        case `concept`:
        case `class`: {
            if (returnVal.hasOwnProperty("constructor")) {
                if (returnVal.copyable && returnVal.movable) {
                    returnVal.constructor.briefDescription = `[[${returnVal.name}]] is copyable and movable.`
                }
                else if (returnVal.copyable && !returnVal.movable) {
                    returnVal.constructor.briefDescription = `[[${returnVal.name}]] is copyable, but not movable.`
                    returnVal.constructor.detailedDescription.push(`[[${returnVal.name}]] is not movable.`)
                }
                else if (!returnVal.copyable && returnVal.movable) {
                    returnVal.constructor.briefDescription = `[[${returnVal.name}]] is not copyable, but is movable.`
                    returnVal.constructor.detailedDescription.push(`[[${returnVal.name}]] is not copyable.`)
                }
                else if (!returnVal.copyable && !returnVal.movable) {
                    returnVal.constructor.briefDescription = `[[${returnVal.name}]] is not copyable nor movable.`
                    returnVal.constructor.detailedDescription.push(`[[${returnVal.name}]] is not copyable nor movable.`)
                }
            }

            returnVal.memberGroups = await Promise.all(
                returnVal.memberGroups.map(async memberGroup => {
                    return {
                        name: memberGroup.name,
                        items: await Promise.all(
                            memberGroup.items.map(
                                async member => {
                                    return await formatRawEntity({
                                        ...member,
                                        project: data.project,
                                        header: data.header,
                                        owners: [...data.owners, data.name],
                                    })
                                }
                            )
                        )
                    }
                })
            )
        } break
        default: break
    }
    return returnVal
}
export async function getEntity(project, header, entity) {
    return await formatRawEntity(await getEntityRaw(project, header, entity))
}
export async function postEntity(project, header, entity, data) {
    return await postJson(`${API_DOCS_URL}${project}/${header}/${entity}`, data)
}

export async function getOverview() {
    return await getJson(`${DATABASE_URL}api/source`)
}

export async function getPathTo(name) {
    const overview = await getOverview()

    let memberSplitterIndex = name.indexOf("::")
    let isMember = memberSplitterIndex >= 0
    let memberName = !isMember ? "" : name.substring(memberSplitterIndex + 2, name.length).split("::").map(x => betterEncodeURIComponent(x)).join("/")
    if (isMember) {
        name = name.substring(0, memberSplitterIndex)

        if (name == `std`) return undefined
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
