#!/usr/bin/env php
<?php
$filename = $argv[1];

// load the document
$doc = new DOMDocument();
$doc->loadHTMLFile($filename);
$xpath = new DOMXpath($doc);

// remove left pane, splitter, search and navigation
$idsToRemove = ["left", "splitter", "search", "navigation"];
foreach($idsToRemove as $id)
{
    $el = $doc->getElementById($id);
    $el->parentNode->removeChild($el);
}

// do not load elementlist script
$els = $xpath->query("//script");
foreach($els as $el)
    $el->parentNode->removeChild($el);

// add lightweight script for expanding methods
$script = $doc->createElement("script");
$script->textContent = "
document.querySelectorAll('.summary.methods tr').forEach(el => {
    el.addEventListener('click', () => {
        el.querySelector('.description.short').classList.add('hidden')
        el.querySelector('.description.detailed').classList.remove('hidden')
    }, {
        once: true,
        passive: true
    })
})
";
$xpath->query("//body")->item(0)->appendChild($script);

// disable sorting in the methods table
$els = $xpath->query("//*[@id='methods']//tr");
foreach($els as $el)
    $el->removeAttribute("data-order");

// add a clarification to the methods table
$xpath->query("//*[@id='methods']/caption")->item(0)->nodeValue .= " (click to expand)";

// remove links from method names
$els = $xpath->query("//*[@id='methods']//*[@class='name']/div/code/a");
foreach($els as $el)
{
    $newNode = $doc->createElement("strong");
    $newNode->textContent = $el->textContent;
    $newNode->setAttribute("class", "method-name");
    $el->parentNode->replaceChild($newNode, $el);
}

// remove links to other pages
$els = $xpath->query("//a");
foreach($els as $el)
{
    $href = $el->getAttribute("href");
    if(!$href || preg_match("~^\\w+:~", $href))
        continue;
    $href = preg_replace("~^[^#]+~", "", $href);
    $el->setAttribute("href", $href);
}

// remove unneeded info
$labels = ["Package:", "Located at"];
foreach($labels as $label)
{
    $el = $xpath->query("//*[@class='info']/*[contains(.,'$label')]")->item(0); // get label
    $parent = $el->parentNode;

    // remove everything up to <br>
    $elsToRemove = [];
    while(true)
    {
        $elsToRemove[] = $el;
        if($el->nodeName == "br")
            break;
        $el = $el->nextSibling;
    }
    foreach($elsToRemove as $el)
        $parent->removeChild($el);
}

// change the header and the title
$header = $doc->getElementsByTagName("h1")->item(0);
$title = preg_replace("~^Class ~", "", $header->textContent);
$title = "$title - API reference";
$header->textContent = $title;
$doc->getElementsByTagName("title")->item(0)->textContent = $title;

// change path to css
$el = $xpath->query("//*[@rel='stylesheet']")->item(0);
$href = $el->getAttribute("href");
$href = preg_replace("~^resources/~", "", $href);
$el->setAttribute("href", $href);

// save the resulting HTML
$doc->saveHTMLFile($filename);
