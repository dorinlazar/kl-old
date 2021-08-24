# Format definition for Markwrite

The purpose of this document is to define the Markwrite (inspired after Markdown) file format. Markwrite is meant as the
main publishing format for the `klblog` engine.

## Short comparison with Markdown

Markdown is meant to be a format that easily translates to the web, while at the same time keeping a pleasant text
representation of the documents. We want to use that property and stray as little as possible away from it; however, for
content management, we need an enriched version that supports extensibility, while not becoming an HTML dumpster. We
still want to be pleasant looking for text-only readers and editors, while at the same time being easy to extend.

Unlike Markdown, Markwrite:
- has an optional metadata field that the writer can use to override auto-detected properties of the document. This will
  allow the user to change things like numbering styles for chapters, author metadata, publishing time, visual metadata
  attached, etc.
- will follow a stricter set of rules regarding what should come where. Markdown has still some ambiguity, and while
  [CommonMark specs](https://spec.commonmark.org/) try to fix that. Some features will be removed from Markdown, most
  likely the HTML embedding logic.
- will allow unified syntax for embedding of external resources (pictures, videos, or any type of widget).

Like Markdown, Markwrite:
- will maintain a clean look for the text files.
- will be easy to understand and maintain.
- will allow authors to focus on content rather than looks.

Markwrite will look like clean Markdown with extra-syntax for embedding resources. Your current Markdown file will
probably be an excellent Markwrite file.

## Markwrite format

This format definition is work in progress. 
