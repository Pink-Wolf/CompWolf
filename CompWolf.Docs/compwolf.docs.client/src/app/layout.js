import "@/styles/globals.css"
import Link from "next/link"
import { getOverview } from "@/lib/api/getEntity"
import MenuTree from "@/lib/MenuTree"
import { Reference } from "@/lib/CodeComponents"
import betterEncodeURIComponent, { decodeStaticParamsEncoder } from "@/lib/betterEncodeURIComponent"

export const dynamic = 'force-static'

export const metadata = {
    title: "CompWolf Docs",
    description: "The page for CompWolf documentation.",
}

async function HeaderMenu() {
    const overview = await getOverview()
    const apiItems = overview.projects.map(project => {
        return {
            name: project.name,
            items: project.headers.map(header => {
                return {
                    name: header.name,
                    items: header.entities.map(entity => {
                        return {
                            name: betterEncodeURIComponent(entity.name),
                        }
                    }),
                }
            }),
            Display: ({ children, path }) => Reference({
                path: `${path}`,
                children: decodeStaticParamsEncoder(children),
            }),
        }
    })
    const data = {
        items: [
            {
                Display: ({ }) => <Link href="/">Home</Link>,
            },
            {
                name: `api`,
                items: apiItems,
                Display: ({ }) => <Link href="/api/">API</Link>,
            }
        ],
        pathPrefix: `/`,
    }

    return MenuTree(data)
}

export default function RootLayout({ children }) {
    return (
        <html lang="en">
            <body>
                <HeaderMenu />
                {children}
            </body>
        </html>
    )
}
