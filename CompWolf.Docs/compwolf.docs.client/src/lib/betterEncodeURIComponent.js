export default function betterEncodeURIComponent(str) {
    // code taken from https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/encodeURIComponent > Encoding for RFC3986
    return encodeURIComponent(str).replace(
        /[!'()*\:]/g,
        (c) => `%${c.charCodeAt(0).toString(16).toUpperCase()}`,
    )
}

// Currently unused as it is decodeStaticParamsEncoder and not generateStaticParamsEncoder that changes depending on GENERATE_STATIC_PARAMS_AUTO_ENCODES
function encodeURIWhenAutoEncoded(str) {
    return str.replace(
        /\:/g,
        (c) => `%${c.charCodeAt(0).toString(16).toUpperCase()}`,
    )
}

export function generateStaticParamsPath(str) {
    return (process.env.GENERATE_STATIC_PARAMS_AUTO_ENCODES === "1")
        ? betterEncodeURIComponent(betterEncodeURIComponent(str))
        : betterEncodeURIComponent(str)
}

export function generateStaticParamsEncoder(str) {
    return betterEncodeURIComponent(str)
}

export function decodeStaticParamsEncoder(str) {
    return (process.env.GENERATE_STATIC_PARAMS_AUTO_ENCODES === "1")
        ? decodeURIComponent(decodeURIComponent(str))
        : decodeURIComponent(str)
}