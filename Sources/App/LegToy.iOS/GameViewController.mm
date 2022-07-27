////////////////////////////////////////////////////////////////////////////////
// GameViewController.mm (LegToy - iOS)
//
// Game View Controller Implementation
////////////////////////////////////////////////////////////////////////////////

#import "GameViewController.h"

#include "LegToyiOSSetting.h"


@implementation GameViewController
{
    
}


- (id)init
{
    self = [super init];
    if (self != nil)
    {
        m_isLandscape = LegToy::Setting::kIsLandscapeMode;
    }
    return self;
}

- (void)viewDidLoad
{
    m_isLandscape = LegToy::Setting::kIsLandscapeMode;
    [super viewDidLoad];
}


- (id)getInitialShowingUIView
{
    // Start color view
    UIView *creatingView = [[UIView alloc] init];
    creatingView.layer.backgroundColor = [[UIColor colorWithRed:0.105882f green:0.372549 blue:0.525490f alpha:1.0f] CGColor];
    
    return creatingView;
}

@end
