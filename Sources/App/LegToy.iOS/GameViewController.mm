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

@end
