import Link from "next/link"

export default function Home() {
    return (
        <div>
            <h1>!!! CompWolf is currently early in development !!!</h1>
            <h1>Welcome to the official CompWolf website.</h1>
            <section>CompWolf is a set of decoupled C++ libraries that aims to allow game developers to create the technical foundation of their game, where the foundation prioritizes (in the given order):
                <ol>
                    <li>correctness and readability,</li>
                    <li>time-to-develop,</li>
                    <li>performance.</li>
                </ol>
            </section>
            <section>Compared to engines like Unity and Unreal, CompWolf aims to be less restrictive for your game&apos;s architecture.
                CompWolf is also build to work with the C++ standard library and other general C++ tools, instead of only tools specifically made for CompWolf.
            </section>
            <h1>Links</h1>
            <p>To learn how to use the CompWolf libraries, consider going through <Link href="/tutorial">these tutorials</Link>.</p>
            <p>If you wish to find specific information about CompWolf, consider checking out <Link href="/api/">its api documentation.</Link>.</p>
            <p>If you wish to see CompWolf&apos;s source code, check out its <Link href="https://github.com/Pink-Wolf/BachelorProject">GitHub repository</Link>.</p>
        </div>
    )
}
