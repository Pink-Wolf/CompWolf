import Link from "next/link"

export default function Home() {
    return (
        <div>
            <h1>Welcome to the official CompWolf website.</h1>
            <section> CompWolf is a bachelor project, which has the minimal viable product for a C++ library used to create windows and objects shown on the windows.
                It currently contains the capacity for drawing squares. It can also take shaders created by you, to draw more complex objects. </section>
            <section> The project also includes a library with general logic like a type list and implementation of the observer pattern. </section>
            <section> The project finally includes this website, containing <Link href="/api/">api documentation</Link> loaded directly from the C++ code.
                This website is made using a React frontend and ASP.NET backend. </section>
            <h1>Links</h1>
            <p><Link href="/api/">API Documentation.</Link>.</p>
            <p><Link href="https://github.com/Pink-Wolf/CompWolf">GitHub Repository</Link>.</p>
        </div>
    )
}
