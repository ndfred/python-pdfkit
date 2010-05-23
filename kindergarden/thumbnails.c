#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

int generateThumbnails(const char *documentPath) {
	int error = 0;
	CFStringRef path = CFStringCreateWithCString(
		NULL, documentPath, kCFStringEncodingUTF8
	);
	CFURLRef url = CFURLCreateWithFileSystemPath(
		NULL, path, kCFURLPOSIXPathStyle, 0
	);
	CGPDFDocumentRef document = CGPDFDocumentCreateWithURL(url);

	if (document != NULL) {
		size_t lastPageIndex = CGPDFDocumentGetNumberOfPages(document) + 1;
		const CGSize thumbnailSize = CGSizeMake(100.0, 150.0);
		CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

		for(size_t pageIndex = 1; pageIndex < lastPageIndex; ++pageIndex) {
			CGPDFPageRef page = CGPDFDocumentGetPage(document, pageIndex);
			CGImageRef image = NULL;
			CFStringRef thumbnailPath = CFStringCreateWithFormat(
				NULL, NULL, CFSTR("%lu.png"), pageIndex
			);
			CFURLRef thumbnailUrl = CFURLCreateWithFileSystemPath(
				NULL, thumbnailPath, kCFURLPOSIXPathStyle, 0
			);
			CGImageDestinationRef destination = CGImageDestinationCreateWithURL(
				thumbnailUrl, kUTTypePNG, 1, NULL
			);
			const CGSize cropBoxSize = CGPDFPageGetBoxRect(page, kCGPDFCropBox).size;
			CGRect thumbnailRect = CGRectMake(0.0, 0.0, 0.0, 0.0);

			if (cropBoxSize.height / cropBoxSize.width <
			    thumbnailSize.height / thumbnailSize.width) {
				thumbnailRect.size = CGSizeMake(
					thumbnailSize.width,
					cropBoxSize.height * thumbnailSize.width / cropBoxSize.width
				);
			} else {
				thumbnailRect.size = CGSizeMake(
					cropBoxSize.width * thumbnailSize.height / cropBoxSize.height,
					thumbnailSize.height
				);
			}

			thumbnailRect = CGRectIntegral(thumbnailRect);

			CGAffineTransform transform = CGPDFPageGetDrawingTransform(
				page, kCGPDFCropBox, thumbnailRect, 0, 0
			);

			CGContextRef context = CGBitmapContextCreate(
				NULL,
				thumbnailRect.size.width, thumbnailRect.size.height,
				8, thumbnailRect.size.width * 4,
				colorSpace, kCGImageAlphaNoneSkipLast
			);

			CGContextSetRGBFillColor(context, 1.0, 1.0, 1.0, 1.0);

			printf("Creating thumbnail for page %lu\n", pageIndex);
			CGContextFillRect(context, thumbnailRect);
			CGContextConcatCTM(context, transform);
			CGContextDrawPDFPage(context, page);
			image = CGBitmapContextCreateImage(context);
			CGImageDestinationAddImage(destination, image, NULL);
			CGImageDestinationFinalize(destination);

			CGContextRelease(context);
			if (destination != NULL) CFRelease(destination);
			if (image != NULL) CFRelease(image);
			if (thumbnailUrl != NULL) CFRelease(thumbnailUrl);
			if (thumbnailPath != NULL) CFRelease(thumbnailPath);
		}

		CGColorSpaceRelease(colorSpace);
		CGPDFDocumentRelease(document);
	} else {
		fprintf(stderr, "Cannot open PDF document\n");
		error = 1;
	}

	if(url != NULL) CFRelease(url);
	if(path != NULL) CFRelease(path);

	return error;
}

int main(int argc, char *argv[]) {
	int error = 0;

	if (argc != 2) {
		fprintf(stderr, "Please specify a PDF file name\n");
		error = 1;
	} else {
		const char *documentPath = argv[1];
		if (generateThumbnails(documentPath) != 0) {
			fprintf(stderr, "Cannot generate thumbnails\n");
			error = 2;
		}
	}

	return error;
}
