import { CodeViewer, SimpleReference } from "@/lib/CodeComponents"

export default function Standards() {
    return (
        <div>
            <h1>Library Standards</h1>
            <section>This page denotes the standards used by the CompWolf C++ libraries.
                Fundamentally, CompWolf must be:
                <ol>
                    <li>Correct</li>
                    <li>Easy to use</li>
                    <li>Quick to use</li>
                    <li>Be usable with third party tools</li>
                    <li>Performant</li>
                </ol>
            </section>
            <section>Code must be readable and correct, including:
                <ul>
                    <li>Names must describe the entity, and not use abbreviations outside of commonly known abbreviations like GPU.</li>
                    <li>Function parameters must be const correct.</li>
                </ul>
            </section>
            <h2>Naming Conventions</h2>
            <section>CompWolf uses the same naming conventions as the C++ standard library, that is:
                <ul>
                    <li>Except template parameters, all names are written as snake_case (all lowercase; words are separated by an underscore).</li>
                    <li>Template parameters are written as PascalCase (first letter of each word is uppercase; other letters are lowercase.).</li>
                    <li>For example:
                        <CodeViewer>
{`float score_multiplier = 1.5f;
template <typename ContainedType, int PixelWidth, int PixelHeight> class text_box;
void apply_poison(float damage_per_second, float duration);`}
                        </CodeViewer>
                    </li>
                </ul>
            </section>
            <section>
                Getter-methods should be named &quot;[variable name]&quot;, while setter-methods should be named &quot;set_[variable name]&quot;.
            </section>
            <section>
                <section>
                    Types, whose sole responsibility is containing data, should be structs;
                    Meta-programming types should be structs;
                    Other types should be classes.
                </section>
                <section>
                    Structs should be aggregates; non-aggregate&apos;s data members should be private.
                    Private data-members may be accessed by a getter-method returning a reference to the member.
                </section>
            </section>
            <section>All entities must have a comment above them describing the entity.
                    <ul>
                        <li>The comment must be written in the JavaDocs style:
                            <ul>
                                <li>The comment must be a multi-line comment, as in: /* comment */.</li>
                                <li>The comment must start by describing the entity in general, followed by extra, more specific information.</li>
                                <li>The following table lists some types of information.
                                    These types of info must have the specified prefix.
                                    These types of info must be ordered as in the table; these types of info must be the last info in the comment.
                                    <table>
                                        <thead>
                                            <tr>
                                                <th>Type</th>
                                                <th>Prefix</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <tr>
                                                <td>A Template Parameter</td>
                                            <td>&quot;@typeparam [name of parameter] &quot;</td>
                                            </tr>
                                            <tr>
                                                <td>A Parameter:</td>
                                                <td>&quot;@param [name of parameter] &quot;</td>
                                            </tr>
                                            <tr>
                                                <td>The function&qyot;s return values:</td>
                                                <td>&quot;@return &quot;</td>
                                            </tr>
                                            <tr>
                                                <td>An exception that may be thrown by the function:</td>
                                                <td>&quot;@throws [type of exception] &quot;</td>
                                            </tr>
                                            <tr>
                                                <td>Related entities:</td>
                                                <td>&quot;@see &quot;</td>
                                            </tr>
                                        </tbody>
                                    </table>
                                </li>
                            </ul>
                        </li>
                    </ul>
            </section>
            <section>
                Use 4 spaces as indention (not tabs).
                A line in source code should be no longer than 80 characters wide, unless it results in notably worse readability.
            </section>
            <h2>Design</h2>
            CompWolf&apos;s design should be decoupled and follow SOLID.
        </div>
    )
}
