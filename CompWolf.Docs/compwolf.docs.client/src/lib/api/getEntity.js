import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

const DATABASE_URL = `https://localhost:7112/`

async function getJson(path) {
    const response = await fetch(path, {
        method: 'GET',
        cache: (process.env.INCLUDE_API_EDITOR == "1") ? `no-cache` : `default`,
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
        body: JSON.stringify(data),
    })
    if (!response.ok) throw new Error(`Could not post data to ${path}\n${response.status}: ${response.statusText}`)
    return await response.json()
}

export async function getExample(name) {
    const example = await getJson(`${DATABASE_URL}examples/${name}`)
    return example
}

export async function getProject(project) {
    const data = await getJson(`${DATABASE_URL}api/${project}`)
    if (data == null) return null
    return {
        ...data,
        project: project,
    }
}
export async function postProject(project, data) {
    return await postJson(`${DATABASE_URL}api/${project}`, data)
}

export async function getHeader(project, header) {
    const data = await getJson(`${DATABASE_URL}api/${project}/${header}`)
    if (data == null) return null
    return {
        ...data,
        project: project,
        header: header,
    }
}
export async function postHeader(project, header, data) {
    return await postJson(`${DATABASE_URL}api/${project}/${header}`, data)
}

export async function getEntityRaw(project, header, entity) {
    const data = await getJson(`${DATABASE_URL}api/${project}/${header}/${entity}`)
    if (data == null) return null
    return {
        ...data,
        project: project,
        header: header,
        name: entity,
        entity: entity,
        owners: [],
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
    return await postJson(`${DATABASE_URL}api/${project}/${header}/${entity}`, data)
}

export async function getOverview() {
    return await getJson(`${DATABASE_URL}api/overview`)
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
